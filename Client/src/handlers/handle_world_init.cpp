#include "app.hpp"
#include "handlers.hpp"
#include "rteng.hpp"

namespace rteng::client_side::handlers {

using Vec = std::vector<std::vector<uint8_t>>;

void handleWorldInit(packet::WorldInit packet,
                     client::HandlerToolbox& toolbox)
{
    LOG_DEBUG("Handling WorldInit.");
    for (size_t i = 0; i < packet.bitsets.size(); i++) {
        const rtecs::DynamicBitSet bitset(packet.bitsets[i]);
        auto& binding_map = toolbox.serverToClient;
        const std::unique_ptr<rtecs::ECS>& ecs = toolbox.engine.getEcs();

        if (binding_map.contains(packet.ids[i])) {
            return;
        }
        LOG_TRACE_R1("Creating new entity.");
        const rtecs::EntityID real = ecs->registerEntity(bitset);
        binding_map.emplace(packet.ids[i], real);
        toolbox.componentFactory.apply(*ecs, real, bitset, packet.entities[i]);
    }
}

}  // namespace rteng::client_side::handlers
