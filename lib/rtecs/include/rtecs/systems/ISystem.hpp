#pragma once

#include "rtecs/bitset/DynamicBitSet.hpp"

namespace rtecs {

class ECS;  // Forward declaration for ECS type

namespace systems {

class ISystem
{
private:
    const bitset::DynamicBitSet _mask;

public:
    virtual ~ISystem() = default;

    /**
     * @brief Apply the system.
     *
     * @param ecs The ECS instance from which this system is called.
     */
    virtual void apply(ECS& ecs) = 0;

    /**
     * @brief Get the name of the system.
     *
     * @return A const-reference of the system's name.
     */
    virtual const std::string& getName() = 0;
};

}  // namespace systems

}  // namespace rtecs
