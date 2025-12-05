#pragma once

#include <sys/types.h>

#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "rtecs/types.h"

namespace rtecs {

// ================================
//      SparseSet - Definition
// ================================
template <typename Component>
class SparseSet {
public:
    static constexpr size_t kPageSize = 2048;
private:
    using Sparse = std::array<ssize_t, kPageSize>;

    using SparseElement = ssize_t;
    static constexpr SparseElement kNullSparseElement = -1;

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
    // (id - 1) because EntityID start at 1 (so that kNullEntity can be 0)
    return (id - 1) / kPageSize;
}

template <typename Component>
size_t SparseSet<Component>::getSparseIndex(const EntityID id) {
    // (id - 1) because EntityID start at 1 (so that kNullEntity can be 0)
    return (id - 1) % kPageSize;
}

template <typename Component>
OptionalRef<Component> SparseSet<Component>::getComponent(const EntityID id) noexcept {
    if (id == 0)
        return std::nullopt;

    const auto page = getPage(id);
    const auto sparseIndex = getSparseIndex(id);

    if (page >= _sparsePages.size()) {
        return std::nullopt;
    }

    const auto denseIndex = _sparsePages[page][sparseIndex];
    return std::ref(_dense[denseIndex]);
}

template <typename Component>
OptionalCRef<Component> SparseSet<Component>::getComponent(const EntityID id) const noexcept {
    if (id == 0)
        return std::nullopt;

    const auto page = getPage(id);
    const auto sparseIndex = getSparseIndex(id);

    if (page >= _sparsePages.size()) {
        return std::nullopt;
    }

    const auto denseIndex = _sparsePages[page][sparseIndex];
    return std::cref(_dense[denseIndex]);
}

template <typename Component>
bool SparseSet<Component>::has(const EntityID id) const noexcept {
    if (id == 0)
        return false;

    const auto page = getPage(id);
    const auto sparseIndex = getSparseIndex(id);

    if (page >= _sparsePages.size())
        return false;
    return _sparsePages[page][sparseIndex] != kNullSparseElement;
}

template <typename Component>
bool SparseSet<Component>::create(const EntityID id, Component component) noexcept {
    if (id == 0)
        return false;

    const auto page = getPage(id);
    const auto sparseIndex = getSparseIndex(id);

    if (page >= _sparsePages.size()) {
        _sparsePages.resize(page + 1);
        _sparsePages[page].fill(kNullSparseElement);
    }

    const SparseElement denseIndex = _sparsePages[page][sparseIndex];

    if (denseIndex == kNullSparseElement) {
        _dense.push_back(component);
        _entities.push_back(id);
        _sparsePages[page][sparseIndex] = _dense.size() - 1;
    } else {
        _dense[denseIndex] = component;
        _entities[denseIndex] = id;
    }
    return true;
}

template <typename Component>
void SparseSet<Component>::destroy(const EntityID id) noexcept {
    if (!has(id))
        return;

    const auto toDestroyPage = getPage(id);
    const auto toDestroySparseIndex = getSparseIndex(id);
    const SparseElement toDestroyElement = _sparsePages[toDestroyPage][toDestroySparseIndex];
    Component &toDestroyComponent = _dense[toDestroyElement];
    Component &toDestroyEntity = _entities[toDestroyElement];

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
