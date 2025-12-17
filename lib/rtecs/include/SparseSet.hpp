#pragma once

#include <sys/types.h>

#include <memory>
#include <optional>
#include <vector>

#include "ISparseSet.hpp"
#include "rtecs/types.hpp"

namespace rtecs {

#define PAGE_OF(id, page_size) (id / page_size)
#define INDEX_OF(id, page_size) (id % page_size)

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
class SparseSet final : public ISparseSet
{
public:
    /**
     * @brief Number of sparse entries in a single page. Tune to balance
     * memory and indexing overhead. Internal indices are computed via
     * page/offset arithmetic.
     */
    static constexpr size_t kPageSize = 2048;

private:
    using SparseElement = size_t;
    using OptionalSparseElement = std::optional<SparseElement>;
    using Sparse = std::array<OptionalSparseElement, kPageSize>;
    static constexpr OptionalSparseElement kNullSparseElement = std::nullopt;

    std::vector<T> _dense;
    std::vector<size_t> _entities;
    std::vector<Sparse> _sparsePages;

    [[nodiscard]]
    static size_t getPage(size_t id);
    [[nodiscard]]
    static size_t getSparseIndex(size_t id);

public:
    /**
     * @brief Get the dense list of entities that possess this component.
     *
     * @returns A vector of the indice for the corresponding entities.
     *
     * Indices match the getAll() vector.
     */
    [[nodiscard]]
    const std::vector<size_t> &getEntities() const noexcept
    {
        return _entities;
    }

    /**
     * @brief Get a reference of the entity.
     *
     * @param id The id of the entity.
     * @return A reference to the component of the entity.
     */
    [[nodiscard]]
    OptionalRef<T> get(size_t id) noexcept;

    /**
     * @brief Get a const-reference of the entity.
     *
     * @param id The id of the entity.
     * @return A const-reference to the component of the entity.
     */
    [[nodiscard]]
    OptionalCRef<T> get(size_t id) const noexcept;

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
     * sparse-set.<br> Note: The first entity id is 1, not 0 !
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
};

// ====================================
//      SparseSet - Implementation
// ====================================
template <typename Component>
size_t SparseSet<Component>::getPage(const size_t id)
{
    return id / kPageSize;
}

template <typename Component>
size_t SparseSet<Component>::getSparseIndex(const size_t id)
{
    return id % kPageSize;
}

template <typename Component>
OptionalRef<Component> SparseSet<Component>::get(const size_t id) noexcept
{
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        return std::nullopt;
    }

    const OptionalSparseElement optionalDenseIndex = _sparsePages[page][sparseIndex];

    if (!optionalDenseIndex.has_value()) {
        return std::nullopt;
    }
    return _dense[optionalDenseIndex.value()];
}

template <typename Component>
OptionalCRef<Component> SparseSet<Component>::get(const size_t id) const noexcept
{
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        return std::nullopt;
    }

    const OptionalSparseElement optionalDenseIndex = _sparsePages[page][sparseIndex];

    if (!optionalDenseIndex.has_value()) {
        return std::nullopt;
    }
    return std::cref(_dense[optionalDenseIndex.value()]);
}

template <typename Component>
std::vector<Component> &SparseSet<Component>::getAll() noexcept
{
    return _dense;
}

template <typename Component>
bool SparseSet<Component>::has(const size_t id) const noexcept
{
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        return false;
    }
    return _sparsePages[page][sparseIndex].has_value();
}

template <typename Component>
bool SparseSet<Component>::put(const size_t id,
                               Component component) noexcept
{
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = INDEX_OF(id, kPageSize);

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

template <typename Component>
void SparseSet<Component>::remove(const size_t id) noexcept
{
    if (!has(id)) {
        return;
    }

    const size_t targetPage = PAGE_OF(id, kPageSize);
    const size_t targetSparseIndex = INDEX_OF(id, kPageSize);
    OptionalSparseElement optionalTargetIndex = _sparsePages[targetPage][targetSparseIndex];

    if (!optionalTargetIndex.has_value()) {
        return;
    }

    const size_t targetIndex = optionalTargetIndex.value();
    Component &targetComponent = _dense[targetIndex];
    size_t &targetEntity = _entities[targetIndex];

    Component &lastComponent = _dense.back();
    size_t &lastEntity = _entities.back();

    std::swap(lastComponent, targetComponent);
    std::swap(lastEntity, targetEntity);

    _dense.pop_back();
    _entities.pop_back();
    _sparsePages[targetPage][targetSparseIndex] = kNullSparseElement;
}

template <typename Component>
void SparseSet<Component>::clear() noexcept
{
    _dense.clear();
    _entities.clear();
    _sparsePages.clear();
}

}  // namespace rtecs
