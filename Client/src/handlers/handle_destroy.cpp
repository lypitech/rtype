#include "app.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleDestroy(const Destroy packet,
                   client::HandlerToolbox& toolbox)
{
    auto& binding_map = toolbox.serverToClient;
    rtecs::types::EntityID real;

    if (binding_map.contains(packet.id)) {
        real = binding_map.at(packet.id);
        binding_map.erase(packet.id);
    }
    toolbox.engine.destroyEntity(real);
    LOG_TRACE_R3("Removed entity {}", real);
}

}  // namespace packet::handler
