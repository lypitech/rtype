#include "apply_bullet_movement.hpp"

#include <cmath>

#include "components/move_set.hpp"
#include "components/position.hpp"
#include "components/type.hpp"
#include "components/velocity.hpp"
#include "enums/move_set.hpp"
#include "lobby/lobby.hpp"
#include "rtecs/ECS.hpp"

namespace server::systems {

ApplyBulletMovement::ApplyBulletMovement(Lobby& lobby)
    : ASystem("ApplyBulletMovement"),
      _lobby(lobby)
{
}

void ApplyBulletMovement::apply(rtecs::ECS& ecs)
{
    using namespace components;
    ecs.group<Position, Type>().apply(
        [this](const rtecs::types::EntityID& id, Position& pos, const Type& type) {
            if (type.type != entity::Type::kBullet) {
                return;
            }
            LOG_TRACE_R1("Here: {}, {}", pos.x, pos.y);
            pos.x += 20;
            _lobby.broadcast(packet::UpdatePosition{id, pos.x, pos.y, 20, 0});
            pos.isUpdated = true;
        });
}

}  // namespace server::systems
