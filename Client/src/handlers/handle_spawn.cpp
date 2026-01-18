#include "app.hpp"
#include "components/animations.hpp"
#include "components/sprite.hpp"
#include "components/target_pos.hpp"
#include "gui/assetManager.hpp"
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

    const gui::AnimationConfig& config = gui::typeToAnimation(type.value().get().type);
    if (config.frameCount > 1) {
        toolbox.engine.getEcs()->addEntityComponents<components::Animation>(id,
                                                                            {config.frameCount,
                                                                             0,
                                                                             config.frameTime,
                                                                             0.0f,
                                                                             config.frameWidth,
                                                                             config.frameHeight,
                                                                             config.loop});
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
    LOG_TRACE_R2("Handling Spawn packet...");
    auto& binding_map = toolbox.serverToClient;
    const std::unique_ptr<rtecs::ECS>& ecs = toolbox.engine.getEcs();

    if (binding_map.contains(packet.id)) {
        LOG_TRACE_R3("Entity has already been created, ignoring...");
        return;
    }
    const rtecs::types::EntityID real = toolbox.engine.getEcs()->preRegisterEntity();
    LOG_TRACE_R3("Spawning the server entity with id {}({})", packet.id, real);
    binding_map.emplace(packet.id, real);
    using Bitset = rtecs::bitset::DynamicBitSet;
    toolbox.componentFactory.apply(*ecs, real, Bitset::deserialize(packet.bitmask), packet.content);
    addGraphicalComponents(real, toolbox);
    LOG_TRACE_R2("Finished recomposing entity {}", real);
}
}  // namespace packet::handler
