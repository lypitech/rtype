#include "app.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleDestroy(const Destroy packet,
                   client::HandlerToolbox& toolbox)
{
    auto& binding_map = toolbox.serverToClient;

    if (!binding_map.contains(packet.id)) {
        LOG_TRACE_R3("Cannot remove entity {}, does not exist.", packet.id);
        return;
    }
    rtecs::types::EntityID real = binding_map.at(packet.id);
    binding_map.erase(packet.id);
    toolbox.engine.destroyEntity(real);
    LOG_TRACE_R3("Removed entity {}", real);
}

}  // namespace packet::handler
