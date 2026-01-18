#include "apply_enemy_movement.hpp"

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

namespace server::systems {

ApplyEnemyMovement::ApplyEnemyMovement()
    : ASystem("ApplyEnemyMovement")
{
}

void ApplyEnemyMovement::apply(rtecs::ECS& ecs)
{
    auto entities = ecs.group<components::Position, components::Velocity, components::MoveSet>();

    entities.apply([](const rtecs::types::EntityID&,
                      const components::Position& position,
                      components::Velocity& velocity,
                      const components::MoveSet& moveSet) {
        if (moveSet.set == static_cast<uint8_t>(move::Set::kStraightSlow)) {
            straightSlow(position, velocity);
            return;
        }
    });
}

}  // namespace server::systems
