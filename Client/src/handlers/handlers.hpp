#pragma once
#include <memory>

#include "packets/server/destroy.hpp"
#include "packets/server/join_ack.hpp"
#include "packets/server/spawn.hpp"
#include "packets/server/update_entity_state.hpp"
#include "packets/server/update_game_state.hpp"
#include "packets/server/update_health.hpp"
#include "packets/server/update_position.hpp"
#include "packets/server/world_init.hpp"
#include "rtnt/core/session.hpp"

namespace client {
using SessionPtr = std::shared_ptr<rtnt::core::Session>;
struct HandlerToolbox;
}  // namespace client

namespace packet::handler {

/**
 * @brief Spawns an entity and adds it's id to the map.
 * @param packet A copy of a spawn packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleSpawn(Spawn packet,
                 client::HandlerToolbox& toolbox);

/**
 * @brief Updates the position of an entity.
 * @param packet A copy of a updatePosition packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleUpdatePosition(UpdatePosition packet,
                          client::HandlerToolbox& toolbox);

/**
 * @brief Initializes a world based on the packet content.
 * @param packet A copy of a worldInit packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleWorldInit(WorldInit packet,
                     client::HandlerToolbox& toolbox);

/**
 * @brief Destroy the entity specified in the packet.
 * @param packet A copy of a destroy packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleDestroy(Destroy packet,
                   client::HandlerToolbox& toolbox);

/**
 * @brief Acknowledges the join request fulfillment.
 * @param packet A copy of a joinAck packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleJoinAck(JoinAck packet,
                   client::HandlerToolbox& toolbox);

/**
 * @brief Updates the current game state.
 * @param packet A copy of a UpdateGameState packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleUpdateGameState(UpdateGameState packet,
                           client::HandlerToolbox& toolbox);

/**
 * @brief Updates the health of the specified entity.
 * @param packet A copy of a UpdateHealth packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleUpdateHealth(UpdateHealth packet,
                        const client::HandlerToolbox& toolbox);

/**
 * @brief Updates the state of the specified entity.
 * @param packet A copy of a UpdateEntity packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleUpdateEntityState(UpdateEntityState packet,
                             const client::HandlerToolbox& toolbox);
}  // namespace packet::handler
