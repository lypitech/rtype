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
        if (pos.isUpdated) {
            const packet::UpdatePosition packet = {static_cast<uint32_t>(id),
                                                   static_cast<uint16_t>(pos.x),
                                                   static_cast<uint16_t>(pos.y),
                                                   0,
                                                   0};

            _lobby.broadcast(packet);
            pos.isUpdated = false;
        }
    });
}

