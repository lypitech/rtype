#include "app.hpp"
#include "components/state.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleUpdateEntityState(const UpdateEntityState packet,
                             const client::HandlerToolbox& toolbox)
{
    if (toolbox.serverToClient.contains(packet.id)) {
        LOG_TRACE_R3("Could not update entity {}'s state, doesn't exist.", packet.id);
        return;
    }
    const rtecs::types::EntityID real = toolbox.serverToClient.at(packet.id);
    const rtecs::types::OptionalRef<components::State> stateOpt =
        toolbox.engine.getEntityFromGroup<components::State>(real);
    if (stateOpt) {
        stateOpt.value().get().state = packet.state;
    }
    LOG_TRACE_R3("Updated state of entity {} to {}", real, packet.state);
}

}  // namespace packet::handler
