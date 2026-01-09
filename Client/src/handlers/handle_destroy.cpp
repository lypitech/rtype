#include "app.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleDestroy(const Destroy packet,
                   client::HandlerToolbox& toolbox)
{
    auto& binding_map = toolbox.serverToClient;
    // rtecs::EntityID real;

    if (binding_map.contains(packet.id)) {
        // real = binding_map.at(packet.id);
        binding_map.erase(packet.id);
    }
    // TODO: Wait for the ecs refactor to be done and call destroyEntity.
}

}  // namespace packet::handler
