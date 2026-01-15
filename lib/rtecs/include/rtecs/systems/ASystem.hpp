#pragma once

#include "rtecs/systems/ISystem.hpp"

namespace rtecs::systems {

/**
 * @brief A system is a method that will be called at each call of the ECS::applyAllSystems() method.
 * @note To make this system applied, you have to register it using `ECS::registerSystem()`.
 */
class ASystem : public ISystem
{
private:
    const std::string& _kName;

protected:
    /**
     * @brief Instantiate a new system.
     *
     * @param name The name of the system. (Used for debugging)
     */
    explicit ASystem(const std::string& name);

public:
    /**
     * @brief Get the name of the system.
     *
     * @return The name of the system.
     */
    const std::string& getName() override;
};
}  // namespace rtecs::systems
