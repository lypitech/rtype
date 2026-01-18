#include "components/hitbox.hpp"
#include "components/position.hpp"
#include "enums/input.hpp"
#include "enums/player_state.hpp"
#include "handlers.hpp"
#include "lobby/lobby.hpp"
#include "packets/client/user_input.hpp"
#include "packets/server/update_position.hpp"
#include "rteng.hpp"

static void spawnBullet(const rtecs::types::EntityID,
                        Lobby& lobby,
                        const components::Position pos,
                        const packet::UserInput packet)
{
    if (packet.input_mask & static_cast<uint8_t>(game::Input::kShoot)) {
        using namespace components;
        lobby.spawnEntity<Type, Position, Owner, Hitbox, Collision, State>(
            {entity::Type::kBullet},
            {pos.x + 100, pos.y + 20, false},
            {214},
            {true, 75, 35},
            {false},
            {entity::state::EntityAlive});
    }
}

namespace packet::handler {

void handleUserInput(const SessionPtr& session,
                     Lobby& lobby,
                     const UserInput& packet)
{
    const rtecs::types::OptionalRef<components::Position>& position =
        lobby.getPlayerPosition(session);

    const rtecs::types::OptionalRef<components::Velocity>& velocity =
        lobby.getPlayerComponent<components::Velocity>(session);

    if (!velocity) {
        LOG_WARN(
            "This should not happen, check for component velocity on this entity or for this "
            "entity presence. (entity id: {})",
            lobby.getPlayerId(session).value());
        return;
    }
    auto& [vx, vy, max_vx, max_vy] = velocity.value().get();

    const std::optional<rtecs::types::EntityID>& id = lobby.getPlayerId(session);
    if (!id) {
        LOG_WARN(
            "This should not be happening, the session may have been removed during the process.");
        return;
    }
    spawnBullet(id.value(), lobby, position.value().get(), packet);
    vx = (packet.input_mask & static_cast<uint8_t>(game::Input::kRight)) ? 10.0f : 0.0f;
    vx += (packet.input_mask & static_cast<uint8_t>(game::Input::kLeft)) ? -10.0f : 0.0f;
    vy = (packet.input_mask & static_cast<uint8_t>(game::Input::kDown)) ? 10.0f : 0.0f;
    vy += (packet.input_mask & static_cast<uint8_t>(game::Input::kUp)) ? -10.0f : 0.0f;
}

}  // namespace packet::handler
