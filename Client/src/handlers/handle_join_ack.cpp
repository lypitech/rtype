#include "app.hpp"
#include "components/me.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleJoinAck(const JoinAck packet,
                   client::HandlerToolbox& toolbox)
{
    const auto& binding_map = toolbox.serverToClient;
    const rtecs::EntityID real = binding_map.at(packet.id);

    auto& me = dynamic_cast<rtecs::SparseSet<components::Me>&>(
        toolbox.engine.getEcs()->getComponent<components::Me>());
    me.put(real);
}

}  // namespace packet::handler
