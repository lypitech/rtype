#include "app.hpp"
#include "components/me.hpp"
#include "enums/game_state.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleJoinAck(const JoinAck packet,
                   client::HandlerToolbox& toolbox)
{
    const auto& binding_map = toolbox.serverToClient;
    const rtecs::types::EntityID real = binding_map.at(packet.id);

    if (!binding_map.contains(packet.id)) {
        LOG_TRACE_R3("entity {} cannot try to join, entity not created.", packet.id);
        return;
    }
    toolbox.engine.getEcs()->addEntityComponents<components::Me>(real, {});
    toolbox.engine.setGameState(packet.gameState);
}

}  // namespace packet::handler
