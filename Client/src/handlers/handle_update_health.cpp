#include "app.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleUpdateHealth(UpdateHealth packet,
                        const client::HandlerToolbox& toolbox)
{
    LOG_TRACE_R2("Handling UpdateHealth packet...");
    auto& binding_map = toolbox.serverToClient;

    if (!binding_map.contains(packet.id)) {
        LOG_TRACE_R3("Could not update entity {}'s health, doesn't exist.", packet.id);
        return;
    }
    const rtecs::types::EntityID real = binding_map.at(packet.id);
    const rtecs::types::OptionalRef<components::Health> healthOpt =
        toolbox.engine.getEntityFromGroup<components::Health>(real);
    if (healthOpt) {
        healthOpt.value().get().hp = packet.health;
    }
}

}  // namespace packet::handler
