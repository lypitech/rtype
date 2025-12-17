#include "handlers.hpp"
#include "packets/server/update_position.hpp"
#include "rteng.hpp"

namespace rteng::client_side::handlers {

void handleUpdatePosition(const SessionPtr&,
                          const packet::UpdatePosition& packet)
{
    rtecs::EntityID id = GameEngine::getInstance().getBindingMap().at(packet.id);
    rtecs::ISparseSet& positions =
        GameEngine::getInstance().getEcs()->getComponent<comp::Position>();

    if (const auto& position = dynamic_cast<rtecs::SparseSet<comp::Position>&>(positions).get(id)) {
        auto& [x, y] = position.value().get();
        x = packet.position_x;
        y = packet.position_y;
    }
}

}  // namespace rteng::client_side::handlers
