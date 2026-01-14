#pragma once

#include <sys/types.h>

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "ASparseSet.hpp"

namespace rtecs::sparse {

#define PAGE_OF(id, page_size) (id / page_size)
#define PAGE_INDEX_OF(id, page_size) (id % page_size)

// ================================
//      SparseSet - Definition
// ================================

/**
 * @brief Contiguous sparse-set implementation mapping entity ids to
 * component instances.
 *
 * Storage layout:
 * - `_dense` stores component instances compactly (dense array).
 * - `_entities` stores the corresponding entity ids for each dense slot.
 * - `_sparsePages` is a paged sparse array mapping an entity id to the
 *   dense index. Each page is an array of optional indices of size
 *   `kPageSize`.
 *
 * This design allows O(1) average-time `has`, `put`, and `remove` (the
 * `remove` performs a swap-with-last in the dense array). The paged sparse
 * array avoids allocating a huge flat sparse array for large entity ids.
 */
template <typename T>
class SparseSet final : public ASparseSet
{
public:
    /**
     * @brief Number of sparse entries in a single page. Tune to balance
     * memory and indexing overhead. Internal indices are computed via
     * page/offset arithmetic using PAGE_OF() and PAGE_INDEX_OF() macros.
     */
    static constexpr size_t kPageSize = 2048;

private:
    using SparseElement = size_t;
    using OptionalSparseElement = std::optional<SparseElement>;
    using Sparse = std::array<OptionalSparseElement, kPageSize>;
    static constexpr OptionalSparseElement kNullSparseElement = std::nullopt;

    std::vector<T> _dense;
    std::vector<Sparse> _sparsePages{};

public:
    /**
     * @brief Construct a new SparseSet.
     *
     * @tparam T The type contained in the SparseSet.
     */
    explicit SparseSet(const types::ComponentID id)
        : ASparseSet(id) {};

    /**
     * @brief Get a reference of the entity.
     *
     * @param id The id of the entity.
     * @return An optional reference to the component of the entity.
     */
    [[nodiscard]]
    types::OptionalRef<T> get(size_t id) noexcept;

    /**
     * @brief Get a const-reference of the entity.
     *
     * @param id The id of the entity.
     * @return An optional const-reference to the component of the entity.
     */
    [[nodiscard]]
    types::OptionalCRef<T> get(size_t id) const noexcept;

    /**
     * @brief Get all the components instances present in this sparse-set.
     *
     * @return A reference to all the components instances.
     */
    [[nodiscard]]
    std::vector<T> &getAll() noexcept;

    /**
     * @brief Check if the sparse-set has the given entity.
     *
     * @param id The id of the entity.
     * @return `true` if the entity is present in the sparse-set, `false`
     * otherwise.
     */
    [[nodiscard]]
    bool has(size_t id) const noexcept override;

    /**
     * @brief Create / Overwrite the component of the entity to the
     * sparse-set.
     *
     * @param id The entity ID to add.
     * @param component The component to create (optional; defaults to a
     * value-initialized Component).
     * @return `true` if the entity has been created, `false` otherwise.
     */
    bool put(size_t id,
             T component = T{}) noexcept;

    /**
     * @brief Remove the entity associated component from the sparse-set.
     *
     * @param id The entity to remove from the sparse-set.
     */
    void remove(size_t id) noexcept override;

    /**
     * Clear the sparse-set.
     */
    void clear() noexcept override;

    /**
     * @brief Get the number of values stored in the SparseSet.
     *
     * @return The number of values stored in the SparseSet.
     */
    [[nodiscard]]
    size_t size() const noexcept override;
};

// ====================================
//      SparseSet - Implementation
// ====================================
template <typename T>
types::OptionalRef<T> SparseSet<T>::get(const size_t id) noexcept
{
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = PAGE_INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        return std::nullopt;
    }

    const OptionalSparseElement optionalDenseIndex = _sparsePages[page][sparseIndex];

    if (!optionalDenseIndex.has_value()) {
        return std::nullopt;
    }
    return _dense[optionalDenseIndex.value()];
}

template <typename T>
types::OptionalCRef<T> SparseSet<T>::get(const size_t id) const noexcept
{
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = PAGE_INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        return std::nullopt;
    }

    const OptionalSparseElement optionalDenseIndex = _sparsePages[page][sparseIndex];

    if (!optionalDenseIndex.has_value()) {
        return std::nullopt;
    }
    return std::cref(_dense[optionalDenseIndex.value()]);
}

template <typename T>
std::vector<T> &SparseSet<T>::getAll() noexcept
{
    return _dense;
}

template <typename T>
bool SparseSet<T>::has(const size_t id) const noexcept
{
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = PAGE_INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        return false;
    }
    return _sparsePages[page].at(sparseIndex).has_value();
}

template <typename T>
size_t SparseSet<T>::size() const noexcept
{
    return _dense.size();
}

template <typename T>
bool SparseSet<T>::put(const size_t id,
                       T component) noexcept
{
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = PAGE_INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        _sparsePages.resize(page + 1);
        _sparsePages[page].fill(kNullSparseElement);
    }

    const OptionalSparseElement optionalDenseIndex = _sparsePages[page][sparseIndex];

    if (!optionalDenseIndex.has_value()) {
        _dense.push_back(component);
        _entities.push_back(id);
        _sparsePages[page][sparseIndex] = _dense.size() - 1;
    } else {
        const size_t denseIndex = optionalDenseIndex.value();
        _dense[denseIndex] = component;
        _entities[denseIndex] = id;
    }
    return true;
}

template <typename T>
void SparseSet<T>::remove(const size_t id) noexcept
{
    if (!has(id)) {
        return;
    }

    const size_t targetPage = PAGE_OF(id, kPageSize);
    const size_t targetSparseIndex = PAGE_INDEX_OF(id, kPageSize);
    const OptionalSparseElement optionalTargetIndex = _sparsePages[targetPage][targetSparseIndex];

    if (!optionalTargetIndex.has_value()) {
        return;
    }

    const size_t targetIndex = optionalTargetIndex.value();
    T &targetComponent = _dense[targetIndex];
    size_t &targetEntity = _entities[targetIndex];

    T &lastComponent = _dense.back();
    size_t &lastEntity = _entities.back();

    std::swap(lastComponent, targetComponent);
    std::swap(lastEntity, targetEntity);

    _dense.pop_back();
    _entities.pop_back();
    _sparsePages[targetPage].at(targetSparseIndex) = kNullSparseElement;
}

template <typename T>
void SparseSet<T>::clear() noexcept
{
    _dense.clear();
    _entities.clear();
    _sparsePages.clear();
}

}  // namespace rtecs::sparse
