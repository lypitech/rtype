#include "app.hpp"
#include "components/sprite.hpp"
#include "handlers.hpp"
#include "rteng.hpp"

static void addGraphicalComponents(rtecs::EntityID Id,
                                   client::HandlerToolbox& toolbox)
{
    auto& sprites = dynamic_cast<rtecs::SparseSet<components::Sprite>&>(
        toolbox.engine.getEcs()->getComponent<components::Sprite>());
    auto& types = dynamic_cast<rtecs::SparseSet<components::Type>&>(
        toolbox.engine.getEcs()->getComponent<components::Type>());
    if (const rtecs::OptionalRef<components::Type> type = types.get(Id)) {
        sprites.put(Id, {type.value().get().type});
    }
}

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
    addGraphicalComponents(real, toolbox);
}

}  // namespace packet::handler
