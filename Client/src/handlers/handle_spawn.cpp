#include "app.hpp"
#include "components/sprite.hpp"
#include "components/target_pos.hpp"
#include "handlers.hpp"
#include "rteng.hpp"

static void addGraphicalComponents(rtecs::types::EntityID id,
                                   client::HandlerToolbox& toolbox)
{
    rtecs::types::OptionalRef<components::Type> type =
        toolbox.engine.getEcs()->group<components::Type>().getEntity<components::Type>(id);
    if (type) {
        toolbox.engine.getEcs()->addEntityComponents<components::Sprite>(
            id, {type.value().get().type});
    }
    rtecs::types::OptionalRef<components::Position> pos =
        toolbox.engine.getEcs()->group<components::Position>().getEntity<components::Position>(id);
    if (pos) {
        toolbox.engine.getEcs()->addEntityComponents<components::TargetPos>(
            id, {pos.value().get().x, pos.value().get().y});
    }
}

namespace packet::handler {

void handleSpawn(Spawn packet,
                 client::HandlerToolbox& toolbox)
{
    auto& binding_map = toolbox.serverToClient;
    const std::unique_ptr<rtecs::ECS>& ecs = toolbox.engine.getEcs();

    if (binding_map.contains(packet.id)) {
        LOG_TRACE_R3("Entity has already been created, ignoring...");
        return;
    }
    const rtecs::types::EntityID real = 0;  // TODO: Wait for the ecs to enable empty entities.
    LOG_TRACE_R3("Spawning the server entity with id {}({})", packet.id, real);
    binding_map.emplace(packet.id, real);
    // TODO: Wait for the DynamicBitset class to be able to be constructed using a std:vector<uint64_t>
    toolbox.componentFactory.apply(*ecs, real, rtecs::bitset::DynamicBitSet(), packet.content);
    addGraphicalComponents(real, toolbox);
}

}  // namespace packet::handler
