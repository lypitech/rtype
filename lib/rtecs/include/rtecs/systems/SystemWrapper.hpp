#pragma once

#include <functional>

#include "rtecs/systems/ASystem.hpp"

namespace rtecs::systems {

class SystemWrapper final : public ASystem
{
private:
    std::function<void(ECS&)>& _applyFn;

public:
    /**
     * @brief Instantiate a new anonymous system.
     *
     * @param applyFn The apply function to call on system apply.
     * @param name The name of the system.
     */
    explicit SystemWrapper(std::function<void(ECS&)>& applyFn, const std::string& name);

    /**
     * @brief Apply the system.
     *
     * @param ecs The ECS instance the system is called from.
     */
    void apply(ECS& ecs) override;
};

}  // namespace rtecs::systems

