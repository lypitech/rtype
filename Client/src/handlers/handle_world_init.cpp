#include "app.hpp"
#include "handlers.hpp"
#include "rteng.hpp"

namespace packet::handler {

using Vec = std::vector<std::vector<uint8_t>>;

void handleWorldInit(WorldInit,
                     client::HandlerToolbox&)
{
    LOG_DEBUG("Handling WorldInit.");
    for (size_t i = 0; i < packet.bitsets.size(); i++) {
        // TODO: Same as spawn packet handler
        const rtecs::bitset::DynamicBitSet bitset;  // packet.bitsets[i]
        auto& binding_map = toolbox.serverToClient;
        const std::unique_ptr<rtecs::ECS>& ecs = toolbox.engine.getEcs();

        if (binding_map.contains(packet.ids[i])) {
            return;
        }
        LOG_TRACE_R1("Creating new entity.");
        // TODO: Same...
        const rtecs::types::EntityID real = 0;
        binding_map.emplace(packet.ids[i], real);
        toolbox.componentFactory.apply(*ecs, real, bitset, packet.entities[i]);
    }
}

}  // namespace packet::handler
