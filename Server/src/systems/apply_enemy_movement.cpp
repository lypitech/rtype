#include "apply_enemy_movement.hpp"

#include <cmath>

#include "components/move_set.hpp"
#include "components/position.hpp"
#include "components/velocity.hpp"
#include "enums/move_set.hpp"
#include "rtecs/ECS.hpp"

static void straightSlow(const components::Position& pos,
                         components::Velocity& velocity)
{
    if (pos.x < 1900 && velocity.vx == 0) {
        velocity.vx = velocity.max_vx;
    } else if (pos.x > 1900) {
        velocity.vx = -velocity.max_vx / 4;
    }
}

static void wave(const components::Position& pos,
                 components::Velocity& velocity)
{
    constexpr float amplitude = 400.0f;
    constexpr float frequency = 0.01f;

    const float slope = amplitude * frequency * std::cos(pos.x * frequency);

    if (pos.x < 700 && velocity.vx == 0) {
        velocity.vx = velocity.max_vx / 2;
    } else {
        velocity.vx = -velocity.max_vx / 2;
    }
    velocity.vy = slope * velocity.vx;
}

static void zigzag(const rtecs::types::EntityID& id,
                   const components::Position& pos,
                   components::Velocity& velocity)
{
    const float phaseOffset = id * 777;
    constexpr std::array frequAmp = {
        std::make_pair<float, float>(.005, 400),
        std::make_pair<float, float>(.008, 150),
        std::make_pair<float, float>(0.03, 20),
    };

    float totalSlope = 0;
    for (const auto& [f, a] : frequAmp) {
        totalSlope += a * f * std::cos((pos.x + phaseOffset) * f);
    }

    if (velocity.vx == 0) {
        velocity.vx = -velocity.max_vx / 2;
    }
    velocity.vy = totalSlope * velocity.vx;
    if (pos.y < 50.0f) {
        velocity.vy += 20.0f;
    } else if (pos.y > 900.0f) {
        velocity.vy -= 20.0f;
    }
}

namespace server::systems {

ApplyEnemyMovement::ApplyEnemyMovement()
    : ASystem("ApplyEnemyMovement")
{
}

void ApplyEnemyMovement::apply(rtecs::ECS& ecs)
{
    auto entities = ecs.group<components::Position, components::Velocity, components::MoveSet>();

    entities.apply([](const rtecs::types::EntityID& id,
                      const components::Position& position,
                      components::Velocity& velocity,
                      const components::MoveSet& moveSet) {
        if (moveSet.set == static_cast<uint8_t>(move::Set::kStraightSlow)) {
            return straightSlow(position, velocity);
        }
        if (moveSet.set == static_cast<uint8_t>(move::Set::kWave)) {
            return wave(position, velocity);
        }
        if (moveSet.set == static_cast<uint8_t>(move::Set::kZigZag)) {
            return zigzag(id, position, velocity);
        }
    });
}

}  // namespace server::systems
