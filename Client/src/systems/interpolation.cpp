#include "interpolation.hpp"

#include <raylib.h>

#include "ECS.hpp"
#include "components/position.hpp"
#include "components/target_pos.hpp"

namespace systems {

Interpolation::Interpolation(const std::unique_ptr<rtecs::ECS>& ecs)
    : ASystem(ecs->getComponentsBitSet<components::Position,
                                       components::TargetPos>())
{
}

void Interpolation::apply(rtecs::ECS& ecs)
{
    for (const auto& [pos, target] : ecs.view<components::Position, components::TargetPos>()) {
        constexpr float interpolationSpeed = 10.0f;
        const float dt = GetFrameTime();  // From Raylib

        pos.x += (target.x - pos.x) * interpolationSpeed * dt;
        pos.y += (target.y - pos.y) * interpolationSpeed * dt;

        if (std::abs(target.x - pos.x) < 0.1f) {
            pos.x = target.x;
        }
        if (std::abs(target.y - pos.y) < 0.1f) {
            pos.y = target.y;
        }
    }
}

}  // namespace systems
