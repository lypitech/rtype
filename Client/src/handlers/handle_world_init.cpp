#include "app.hpp"
#include "handlers.hpp"
#include "rteng.hpp"

namespace packet::handler {

using Vec = std::vector<std::vector<uint8_t>>;

void handleWorldInit(WorldInit packet,
                     client::HandlerToolbox& toolbox)
{
    LOG_TRACE_R2("Handling WorldInit packet.");
    toolbox.engine.setGameState(packet.state);
    const auto& binding_map = toolbox.serverToClient;
    for (size_t i = 0; i < packet.bitsets.size(); i++) {
        if (binding_map.contains(packet.ids[i])) {
            LOG_TRACE_R1("Entity {} already exists.", packet.ids[i]);
            continue;
        }
        const Spawn s{packet.ids[i], packet.bitsets[i], packet.entities[i]};
        handleSpawn(s, toolbox);
    }
}

}  // namespace packet::handler
