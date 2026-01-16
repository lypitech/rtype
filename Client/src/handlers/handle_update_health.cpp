#include "app.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleUpdateHealth(UpdateHealth packet,
                        const client::HandlerToolbox& toolbox)
{
    const rtecs::types::OptionalRef<components::Health> healthOpt =
        toolbox.engine.getEntityFromGroup<components::Health>(packet.id);
    if (healthOpt) {
        healthOpt.value().get().hp = packet.health;
    }
}

}  // namespace packet::handler
