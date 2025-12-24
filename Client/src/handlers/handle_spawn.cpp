#include "app.hpp"
#include "handlers.hpp"
#include "rteng.hpp"

namespace packet::handler {

void handleSpawn(Spawn packet,
                 client::HandlerToolbox& toolbox)
{
    const rtecs::DynamicBitSet bitset(packet.bitmask);
    auto& binding_map = toolbox.serverToClient;
    const std::unique_ptr<rtecs::ECS>& ecs = toolbox.engine.getEcs();

    if (binding_map.contains(packet.id)) {
        LOG_TRACE_R2("Entity has already been created, ignoring...");
        return;
    }
    const rtecs::EntityID real = ecs->registerEntity(bitset);
    LOG_TRACE_R2("Spawning the server entity with id {}({})", packet.id, real);
    binding_map.emplace(packet.id, real);
    toolbox.componentFactory.apply(*ecs, real, bitset, packet.content);
}

}  // namespace packet::handler
