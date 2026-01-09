#pragma once
#include <memory>

#include "packets/server/spawn.hpp"
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
 * @param packet A reference to a spawn packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleSpawn(Spawn packet,
                 client::HandlerToolbox& toolbox);
// void handleUpdatePosition(const SessionPtr&,
//                           const packet::UpdatePosition& packet);
/**
 * @brief Initializes a world based on the packet content.
 * @param packet A copy of a worldInit packet.
 * @param toolbox A reference to the toolbox stored in the app.
 */
void handleWorldInit(WorldInit packet,
                     client::HandlerToolbox& toolbox);

}  // namespace packet::handler
