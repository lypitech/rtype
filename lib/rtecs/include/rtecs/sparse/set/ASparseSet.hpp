#pragma once

#include "ISparseSet.hpp"
#include "rtecs/bitset/DynamicBitSet.hpp"
#include "rtecs/types/types.hpp"

namespace rtecs::sparse {

/**
 * @brief An abstraction of the ISparseSet.
 *
 * This class implements the ISparseSet::getMask method.
 */
class ASparseSet : public ISparseSet
{
private:
    const types::ComponentID _id;

protected:
    std::vector<size_t> _entities;

public:
    /**
     * @brief Instantiate a new SparseSet.
     *
     * @param id The SparseSet ID.
     */
    explicit ASparseSet(types::ComponentID id);

    /**
     * @brief Get the dense list of entities that possess this component.
     * @note Indices match the getAll() vector.
     * @returns A vector of the indice for the corresponding entities.
     */
    [[nodiscard]]
    const std::vector<types::EntityID> &getEntities() const noexcept override;

    /**
     * @brief Get the ID of the SparseSet.
     *
     * @return The ID of the SparseSet.
     */
    [[nodiscard]]
    types::ComponentID getId() const override;
};

}  // namespace rtecs::sparse
