#include "app.hpp"
#include "components/target_pos.hpp"
#include "handlers.hpp"
#include "packets/server/update_position.hpp"

namespace packet::handler {

void handleUpdatePosition(UpdatePosition packet,
                          client::HandlerToolbox& toolbox)
{
    const rtecs::EntityID id = toolbox.serverToClient.at(packet.id);
    rtecs::ISparseSet& positions = toolbox.engine.getEcs()->getComponent<components::Position>();
    rtecs::ISparseSet& tPos = toolbox.engine.getEcs()->getComponent<components::TargetPos>();
    const auto& posOpt = dynamic_cast<rtecs::SparseSet<components::Position>&>(positions).get(id);
    const auto& targetOpt = dynamic_cast<rtecs::SparseSet<components::TargetPos>&>(tPos).get(id);

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
