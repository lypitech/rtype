#include "enums/input.hpp"
#include "handlers.hpp"
#include "packets/client/user_input.hpp"
#include "packets/server/update_position.hpp"
#include "rteng.hpp"

namespace rteng::server_side::handlers {

void handleUserInput(const SessionPtr& s, const packet::UserInput& packet)
{
    const rtecs::EntityID id = GameEngine::getInstance().getSrvBindingMap().at(s->getId());
    rtecs::ISparseSet& positions = GameEngine::getInstance().getEcs()->getComponent<comp::Position>();

    if (const auto& position = dynamic_cast<rtecs::SparseSet<comp::Position>&>(positions).get(id)) {
        auto& [x, y] = position.value().get();

        x += (packet.input_mask & static_cast<uint8_t>(game::Input::kRight)) ? 10.0f : 0.0f;
        x -= (packet.input_mask & static_cast<uint8_t>(game::Input::kLeft)) ? 10.0f : 0.0f;
        y += (packet.input_mask & static_cast<uint8_t>(game::Input::kDown)) ? 10.0f : 0.0f;
        y -= (packet.input_mask & static_cast<uint8_t>(game::Input::kUp)) ? 10.0f : 0.0f;
        GameEngine::getInstance().getServer()->broadcast(packet::UpdatePosition{
            static_cast<uint32_t>(id), static_cast<uint16_t>(x), static_cast<uint16_t>(y), 0, 0});
    }
}

}  // namespace rteng::server_side::handlers
