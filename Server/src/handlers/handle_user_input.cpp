#include "components/position.hpp"
#include "enums/input.hpp"
#include "handlers.hpp"
#include "lobby/lobby.hpp"
#include "packets/client/user_input.hpp"
#include "packets/server/update_position.hpp"
#include "rteng.hpp"

namespace packet::handler {

void handleUserInput(const SessionPtr& s,
                     Lobby& lobby,
                     const UserInput& packet)
{
    const rtecs::OptionalRef<components::Position>& position = lobby.getPlayerPosition(s);

    if (!position) {
        LOG_WARN(
            "This should not happen, check for component position on this entity or for this "
            "entity presence.");
        return;
    }
    auto& [x, y] = position.value().get();

    x += (packet.input_mask & static_cast<uint8_t>(game::Input::kRight)) ? 10.0f : 0.0f;
    x -= (packet.input_mask & static_cast<uint8_t>(game::Input::kLeft)) ? 10.0f : 0.0f;
    y += (packet.input_mask & static_cast<uint8_t>(game::Input::kDown)) ? 10.0f : 0.0f;
    y -= (packet.input_mask & static_cast<uint8_t>(game::Input::kUp)) ? 10.0f : 0.0f;
    const std::optional<rtecs::EntityID>& id = lobby.getPlayerId(s);
    if (!id) {
        LOG_WARN(
            "This should not be happening, the session may have been removed during the process.");
    }
    const packet::UpdatePosition p = {static_cast<uint32_t>(id.value()),
                                      static_cast<uint16_t>(x),
                                      static_cast<uint16_t>(y),
                                      0,
                                      0};
    lobby.broadcast(p);
}

}  // namespace packet::handler
