#pragma once

#include <cstddef>

namespace rtecs::sparse {

class DynamicBitSet; // Forward class definition

/**
 * @brief Interface for a sparse-set container used by the ECS.
 *
 * `ISparseSet` exposes the minimum operations required by the engine to
 * interact with component storage without coupling to a concrete
 * implementation. Implementations (e.g., `SparseSet<T>`) provide storage for
 * component instances associated with entity ids.
 */
class ISparseSet
{
public:
    virtual ~ISparseSet() = default;

    /**
     * @brief Check if the sparse-set has the given entity.
     *
     * @param id The id of the entity.
     * @return `true` if the entity is present in the sparse-set, `false`
     * otherwise.
     */
    [[nodiscard]]
    virtual bool has(size_t id) const noexcept = 0;

    /**
     * @brief Remove the entity associated component from the sparse-set.
     *
     * Implementations should ensure removing an entity is O(1) where
     * possible (e.g., swap-remove from dense storage).
     *
     * @param id The entity to remove from the sparse-set.
     */
    virtual void remove(size_t id) noexcept = 0;

    /**
     * @brief Clear the sparse-set, removing all stored components.
     */
    virtual void clear() noexcept = 0;
};

}  // namespace rtecs::sparse
