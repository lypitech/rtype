#pragma once

#include "rtecs/systems/ASystem.hpp"

namespace server::systems {

class ApplyMovement final : public rtecs::systems::ASystem
{
private:
    /**
     * @brief Check if a horizontal/vertical collision is detected.
     *
     * @param refPos The reference X/Y position
     * @param refSize The reference box's width/height
     * @param otherPos The other X/Y position
     * @param otherSize The other box's width/height
     * @return `true` if a horizontal/vertical collision is detected, `false` otherwise.
     */
    static bool checkCollision(float refPos,
                               float refSize,
                               float otherPos,
                               float otherSize);

public:
    explicit ApplyMovement();
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace server::systems
