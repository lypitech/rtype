#include "interpolation.hpp"

#include <raylib.h>

#include "components/position.hpp"
#include "components/target_pos.hpp"
#include "rtecs/ECS.hpp"
#include "rtecs/systems/ASystem.hpp"

namespace systems {

Interpolation::Interpolation()
    : ASystem("Interpolation")
{
}

void Interpolation::apply(rtecs::ECS&)
{
    ecs.group<components::Position, components::TargetPos>().apply(
        [](const rtecs::types::EntityID&,
           components::Position& position,
           const components::TargetPos& targetPosition) {
            constexpr float interpolationSpeed = 10.0f;
            const float dt = GetFrameTime();  // From Raylib

            position.x += (targetPosition.x - position.x) * interpolationSpeed * dt;
            position.y += (targetPosition.y - position.y) * interpolationSpeed * dt;

            if (std::abs(targetPosition.x - position.x) < 0.1f) {
                position.x = targetPosition.x;
            }
            if (std::abs(targetPosition.y - position.y) < 0.1f) {
                position.y = targetPosition.y;
            }
        });
}

}  // namespace systems
