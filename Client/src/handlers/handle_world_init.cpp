#include "app.hpp"
#include "handlers.hpp"
#include "rteng.hpp"

namespace packet::handler {

using Vec = std::vector<std::vector<uint8_t>>;

void handleWorldInit(WorldInit packet,
                     client::HandlerToolbox& toolbox)
{
    LOG_DEBUG("Handling WorldInit.");
    for (size_t i = 0; i < packet.bitsets.size(); i++) {
        auto& binding_map = toolbox.serverToClient;
        const std::unique_ptr<rtecs::ECS>& ecs = toolbox.engine.getEcs();

        if (binding_map.contains(packet.ids[i])) {
            return;
        }
        LOG_TRACE_R1("Creating new entity.");
        const rtecs::types::EntityID real = ecs->preRegisterEntity();
        binding_map.emplace(packet.ids[i], real);
        using Bitset = rtecs::bitset::DynamicBitSet;
        toolbox.componentFactory.apply(
            *ecs, real, Bitset::deserialize(packet.bitsets[i]), packet.entities[i]);
    }
}

}  // namespace packet::handler
