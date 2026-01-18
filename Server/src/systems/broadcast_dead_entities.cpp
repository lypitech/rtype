#include "broadcast_dead_entities.hpp"

#include "enums/player_state.hpp"

using namespace server::systems;
using namespace components;

BroadcastDeadEntities::BroadcastDeadEntities(Lobby& lobby)
    : ASystem("BroadcastDeadEntities"),
      _lobby(lobby)
{
}

void BroadcastDeadEntities::apply(rtecs::ECS& ecs)
{
    auto group = ecs.group<State>();

    group.apply([&](const rtecs::types::EntityID id, const State& state) {
        if (state.state == entity::state::EntityDead) {
            LOG_INFO("Killing entity {}.", id);
            _lobby.killEntity(id);
        }
    });
}

