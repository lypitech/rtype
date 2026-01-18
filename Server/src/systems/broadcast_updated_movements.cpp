#include "broadcast_updated_movements.hpp"

#include "components/position.hpp"
#include "lobby/lobby.hpp"

using namespace server::systems;
using namespace components;

BroadcastUpdatedMovements::BroadcastUpdatedMovements(Lobby& lobby)
    : ASystem("BroadcastUpdatedMovements"),
      _lobby(lobby)
{
}

void BroadcastUpdatedMovements::apply(rtecs::ECS& ecs)
{
    auto group = ecs.group<Position>();

    group.apply([&](const rtecs::types::EntityID id, Position& pos) {
        const auto velOpt = ecs.group<Velocity>().getEntity<Velocity>(id);
        if (pos.isUpdated) {
            packet::UpdatePosition packet = {id, pos.x, pos.y, 0, 0};
            if (velOpt) {
                packet.vx = velOpt.value().get().vx;
                packet.vy = velOpt.value().get().vy;
            }

            _lobby.broadcast(packet);
            pos.isUpdated = false;
        }
    });
}

