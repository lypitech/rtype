#include "app.hpp"
#include "components/state.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleUpdateEntityState(const UpdateEntityState packet,
                             const client::HandlerToolbox& toolbox)
{
    LOG_TRACE_R2("Handling UpdateEntityState packet...");
    auto& binding_map = toolbox.serverToClient;

    if (!binding_map.contains(packet.id)) {
        LOG_TRACE_R3("Could not update entity {}'s state, doesn't exist.", packet.id);
        return;
    }
    const rtecs::types::EntityID real = binding_map.at(packet.id);
    const rtecs::types::OptionalRef<components::State> stateOpt =
        toolbox.engine.getEntityFromGroup<components::State>(real);
    if (stateOpt) {
        stateOpt.value().get().state = packet.state;
    }
    LOG_TRACE_R3("Updated state of entity {} to {}", real, packet.state);
}

}  // namespace packet::handler
