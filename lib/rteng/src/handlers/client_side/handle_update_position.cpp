#include "handlers.hpp"
#include "packets/server/update_position.hpp"
#include "rteng.hpp"

namespace rteng::client_side::handlers {

void handleUpdatePosition(const SessionPtr&, const packet::UpdatePosition& packet)
{
    rtecs::EntityID id = GameEngine::getInstance().getBindingMap().at(packet.id);
    rtecs::ISparseSet& positions = GameEngine::getInstance().getEcs()->getComponent<comp::Position>();

    float old_x, old_y;
    if (const auto& position = dynamic_cast<rtecs::SparseSet<comp::Position>&>(positions).get(id)) {
        LOG_DEBUG("Found position, updating...");
        auto& [x, y] = position.value().get();
        old_x = x;
        old_y = y;
        x = packet.position_x;
        y = packet.position_y;
    }
    if (const auto& position = dynamic_cast<rtecs::SparseSet<comp::Position>&>(positions).get(id)) {
        auto& [x, y] = position.value().get();
        LOG_DEBUG("Moved from ({}, {}) to ({}, {})", old_x, old_y, x, y);
    }
}

}  // namespace rteng::client_side::handlers
