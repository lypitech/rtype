#include "app.hpp"
#include "components/me.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleJoinAck(const JoinAck packet,
                   client::HandlerToolbox& toolbox)
{
    const auto& binding_map = toolbox.serverToClient;
    const rtecs::types::EntityID real = binding_map.at(packet.id);

    toolbox.engine.getEcs()->addEntityComponents<components::Me>(real, {});
    // TODO: Maybe change gameState or smthg
}

}  // namespace packet::handler
