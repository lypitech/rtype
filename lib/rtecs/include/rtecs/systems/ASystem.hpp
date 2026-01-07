#pragma once

#include "rtecs/bitset/DynamicBitSet.hpp"

namespace rtecs {

class ECS;  // Forward declaration for ECS type

namespace systems {

class ASystem
{
private:
    const bitset::DynamicBitSet _mask;
public:

    /**
     * @brief Instantiate a new system.
     *
     * @param mask The mask of the components managed by this system.
     */
    explicit ASystem(const bitset::DynamicBitSet &mask);
    virtual ~ASystem() = default;

    /**
     * @brief Get the mask that describe the components managed by this system.
     *
     * @return The components' mask of the system.
     */
    [[nodiscard]]
    const bitset::DynamicBitSet &getMask() const noexcept;

    /**
     * @brief Apply the system.
     *
     * @param ecs The ECS instance from which this system is called.
     */
    virtual void apply(ECS &ecs) = 0;
};

}  // namespace rtecs::system

}  // namespace rtecs
