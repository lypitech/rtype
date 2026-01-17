#include "components/hitbox.hpp"
#include "components/position.hpp"
#include "enums/input.hpp"
#include "handlers.hpp"
#include "lobby/lobby.hpp"
#include "packets/client/user_input.hpp"
#include "packets/server/update_position.hpp"
#include "rteng.hpp"

static void spawnBullet(const rtecs::types::EntityID& id,
                        Lobby& lobby,
                        const components::Position& pos,
                        const packet::UserInput& packet)
{
    if (packet.input_mask & static_cast<uint8_t>(game::Input::kShoot)) {
        using namespace components;
        lobby.spawnEntity<Type, Position, Owner, Velocity, Hitbox, Collision>(
            {entity::Type::kBullet}, {pos.x, pos.y}, {id}, {20, 0}, {true, 75, 35}, {});
    }
}

namespace packet::handler {

void handleUserInput(const SessionPtr& session,
                     Lobby& lobby,
                     const UserInput& packet)
{
    // const rtecs::types::OptionalRef<components::Position>& position =
    //     lobby.getPlayerPosition(session);

    const rtecs::types::OptionalRef<components::Velocity>& velocity =
        lobby.getPlayerComponent<components::Velocity>(session);

    if (!velocity) {
        LOG_WARN(
            "This should not happen, check for component velocity on this entity or for this "
            "entity presence.");
        return;
    }
    auto& [vx, vy] = velocity.value().get();

// <<<<<<< HEAD
    // x += (packet.input_mask & static_cast<uint8_t>(game::Input::kRight)) ? 10.0f : 0.0f;
    // x -= (packet.input_mask & static_cast<uint8_t>(game::Input::kLeft)) ? 10.0f : 0.0f;
    // y += (packet.input_mask & static_cast<uint8_t>(game::Input::kDown)) ? 10.0f : 0.0f;
    // y -= (packet.input_mask & static_cast<uint8_t>(game::Input::kUp)) ? 10.0f : 0.0f;
    // if (y <= 0) {
    //     y = 0;
    // }
    // if (x <= 0) {
    //     x = 0;
    // }
    const std::optional<rtecs::types::EntityID>& id = lobby.getPlayerId(session);
    if (!id) {
        LOG_WARN(
            "This should not be happening, the session may have been removed during the process.");
        return;
    }
    const UpdatePosition p = {static_cast<uint32_t>(id.value()),
                              static_cast<uint16_t>(x),
                              static_cast<uint16_t>(y),
                              0,
                              0};
    spawnBullet(id.value(), lobby, position.value().get(), packet);
    // lobby.broadcast(p);
// =======
    vx = (packet.input_mask & static_cast<uint8_t>(game::Input::kRight)) ? 10.0f : 0.0f;
    vx = (packet.input_mask & static_cast<uint8_t>(game::Input::kLeft)) ? 10.0f : 0.0f;
    vy = (packet.input_mask & static_cast<uint8_t>(game::Input::kDown)) ? 10.0f : 0.0f;
    vy = (packet.input_mask & static_cast<uint8_t>(game::Input::kUp)) ? 10.0f : 0.0f;

    // The system will update the position from the velocity

    // const std::optional<rtecs::types::EntityID>& id = lobby.getPlayerId(session);
    // if (!id) {
    //     LOG_WARN(
    //         "This should not be happening, the session may have been removed during the process.");
    //     return;
    // }
    // const UpdatePosition p = {static_cast<uint32_t>(id.value()),
    //                           static_cast<uint16_t>(x),
    //                           static_cast<uint16_t>(y),
    //                           0,
    //                           0};
    // lobby.broadcast(p);
// >>>>>>> b6c41df (refactor(server): Changed the handle_user_input to update the velocity instead of the position, and to not send anymore the UpdatePosition packet since its sent using the BroadcastUpdatedMovement system.)
}

}  // namespace packet::handler
