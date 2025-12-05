#pragma once

#include <sys/types.h>

#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "rtecs/types.h"

namespace rtecs {

#ifndef PAGED_ARRAY
    #define PAGE_OF(id, page_size) (id / page_size)
    #define INDEX_OF(id, page_size) (id % page_size)
#endif // PAGED_ARRAY

// ================================
//      SparseSet - Definition
// ================================
template <typename Component>
class SparseSet {
public:
    static constexpr size_t kPageSize = 2048;

private:
    using SparseElement = size_t;
    using OptionalSparseElement = std::optional<SparseElement>;
    using Sparse = std::array<OptionalSparseElement, kPageSize>;
    static constexpr OptionalSparseElement kNullSparseElement = std::nullopt;

    std::vector<Component> _dense;
    std::vector<EntityID> _entities;
    std::vector<Sparse> _sparsePages;

    [[nodiscard]] static size_t getPage(EntityID id);
    [[nodiscard]] static size_t getSparseIndex(EntityID id);

public:
    /**
     * @brief Get a reference of the entity.
     *
     * @param id The id of the entity.
     * @return A reference to the component of the entity.
     */
    [[nodiscard]] OptionalRef<Component> getComponent(EntityID id) noexcept;

    /**
     * @brief Get a const-reference of the entity.
     *
     * @param id The id of the entity.
     * @return A const-reference to the component of the entity.
     */
    [[nodiscard]] OptionalCRef<Component> getComponent(EntityID id) const noexcept;

    /**
     * @brief Check if the sparse-set has the given entity.
     *
     * @param id The id of the entity.
     * @return `true` if the entity is present in the sparse-set, `false` otherwise.
     */
    [[nodiscard]] bool has(EntityID id) const noexcept;

    /**
     * @brief Create / Overwrite the component of the entity to the sparse-set.<br>
     * Note: The first entity id is 1, not 0 !
     *
     * @param id The entity ID to add.
     * @param component The component to create (optional; defaults to a value-initialized Component).
     * @return `true` if the entity has been created, `false` otherwise.
     */
    bool create(EntityID id, Component component = Component{}) noexcept;

    /**
     * @brief Remove the entity associated component from the sparse-set.
     *
     * @param id The entity to remove from the sparse-set.
     */
    void destroy(EntityID id) noexcept;

    /**
     * Clear the sparse-set.
     */
    void clear() noexcept;
};

// ====================================
//      SparseSet - Implementation
// ====================================
template <typename Component>
size_t SparseSet<Component>::getPage(const EntityID id) {
    return id / kPageSize;
}

template <typename Component>
size_t SparseSet<Component>::getSparseIndex(const EntityID id) {
    return id % kPageSize;
}

template <typename Component>
OptionalRef<Component> SparseSet<Component>::getComponent(const EntityID id) noexcept {
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        return std::nullopt;
    }

    const OptionalSparseElement optionalDenseIndex = _sparsePages[page][sparseIndex];

    if (!optionalDenseIndex.has_value())
        return std::nullopt;
    return _dense[optionalDenseIndex.value()];
}

template <typename Component>
OptionalCRef<Component> SparseSet<Component>::getComponent(const EntityID id) const noexcept {
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size()) {
        return std::nullopt;
    }

    const OptionalSparseElement optionalDenseIndex = _sparsePages[page][sparseIndex];

    if (!optionalDenseIndex.has_value())
        return std::nullopt;
    return std::cref(_dense[optionalDenseIndex.value()]);
}

template <typename Component>
bool SparseSet<Component>::has(const EntityID id) const noexcept {
    const size_t page = PAGE_OF(id, kPageSize);
    const size_t sparseIndex = INDEX_OF(id, kPageSize);

    if (page >= _sparsePages.size())
        return false;
    return _sparsePages[page][sparseIndex].has_value();
}

template <typename Component>
bool SparseSet<Component>::create(const EntityID id, Component component) noexcept {
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
void SparseSet<Component>::destroy(const EntityID id) noexcept {
    if (!has(id))
        return;

    const size_t toDestroyPage = PAGE_OF(id, kPageSize);
    const size_t toDestroySparseIndex = INDEX_OF(id, kPageSize);
    OptionalSparseElement optionalToDestroyIndex = _sparsePages[toDestroyPage][toDestroySparseIndex];

    if (!optionalToDestroyIndex.has_value())
        return;

    const size_t toDestroyIndex = optionalToDestroyIndex.value();
    Component &toDestroyComponent = _dense[toDestroyIndex];
    EntityID &toDestroyEntity = _entities[toDestroyIndex];

    Component &lastComponent = _dense.back();
    EntityID &lastEntity = _entities.back();

    std::swap(lastComponent, toDestroyComponent);
    std::swap(lastEntity, toDestroyEntity);

    _dense.pop_back();
    _entities.pop_back();
    _sparsePages[toDestroyPage][toDestroySparseIndex] = kNullSparseElement;
}

template <typename Component>
void SparseSet<Component>::clear() noexcept {
    _dense.clear();
    _entities.clear();
    _sparsePages.clear();
}

}
