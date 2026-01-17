#pragma once

#include <optional>

#include "components/hitbox.hpp"
#include "components/position.hpp"
#include "components/velocity.hpp"
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
    static bool collide(float refPos,
                        float refSize,
                        float otherPos,
                        float otherSize);

    static void applyHorizontalMovement(
        components::Position& pos,
        components::Velocity& vel,
        const components::Hitbox& box,
        const std::optional<std::pair<components::Position,
                                      components::Hitbox>>& collider);
    static void applyVerticalMovement(components::Position& pos,
                                      components::Velocity& vel,
                                      const components::Hitbox& box,
                                      const std::optional<std::pair<components::Position,
                                                                    components::Hitbox>>& collider);

public:
    explicit ApplyMovement();
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace server::systems
