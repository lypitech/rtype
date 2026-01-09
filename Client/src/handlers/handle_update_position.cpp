#include "app.hpp"
#include "handlers.hpp"
#include "packets/server/update_position.hpp"

namespace packet::handler {

void handleUpdatePosition(UpdatePosition packet,
                          client::HandlerToolbox& toolbox)
{
    const rtecs::EntityID id = toolbox.serverToClient.at(packet.id);
    rtecs::ISparseSet& positions = toolbox.engine.getEcs()->getComponent<components::Position>();

    if (const auto& position =
            dynamic_cast<rtecs::SparseSet<components::Position>&>(positions).get(id)) {
        auto& [x, y] = position.value().get();
        x = packet.position_x;
        y = packet.position_y;
    }  // TODO: maybe set a "target_position" and use interpolation to smoothen movement.
}

}  // namespace packet::handler
