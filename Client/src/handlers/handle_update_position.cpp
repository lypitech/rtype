#include "app.hpp"
#include "components/target_pos.hpp"
#include "handlers.hpp"
#include "packets/server/update_position.hpp"

namespace packet::handler {

void handleUpdatePosition(UpdatePosition packet,
                          client::HandlerToolbox& toolbox)
{
    LOG_TRACE_R2("Handling UpdatePosition packet...");
    auto& binding_map = toolbox.serverToClient;

    if (!binding_map.contains(packet.id)) {
        LOG_TRACE_R3("Entity {} does not exit, cannot update position", packet.id);
        return;
    }
    const rtecs::types::EntityID id = binding_map.at(packet.id);
    auto positions = toolbox.engine.getEcs()->group<components::Position, components::TargetPos>();
    const auto posOpt = positions.getEntity<components::Position>(id);
    const auto targetOpt = positions.getEntity<components::TargetPos>(id);

    if (posOpt && targetOpt) {
        components::Position& pos = posOpt.value().get();
        components::TargetPos& target = targetOpt.value().get();
        const float dx = packet.x - pos.x;
        const float dy = packet.y - pos.y;
        if (dx * dx + dy * dy > 300.0f * 300.0f) {
            pos.x = packet.x;
            pos.y = packet.y;
        } else {
            target.x = packet.x;
            target.y = packet.y;
        }
    }
}

}  // namespace packet::handler
