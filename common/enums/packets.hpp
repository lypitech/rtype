#pragma once
#include <cstdint>

namespace packet::type {

/**
 * @class Client
 *
 * @brief The id of the packet sent by the client.
 */
enum class Client : uint16_t
{
    kJoin = 0x128,       // Ask to join a game.
    kStart = 0x129,      // Ask to start a game.
    kUserInput = 0x130,  // Send the bitmask of the input.
};

/**
 * @class Server
 *
 * @brief The id of the packet sent by the server.
 */
enum class Server : uint16_t
{
    kJoinAck = 0x150,            // Notify the status of a join request.
    kPlayerJoin = 0x151,         // Notify that a player joined the lobby.
    kSpawn = 0x152,              // Notify of an entity spawn.
    kDestroy = 0x153,            // Notify of an entity destroying.
    kUpdateEntityState = 0x154,  // Update the state of an entity.
    kUpdatePosition = 0x155,     // Update the position of an entity.
    kUpdateHealth = 0x156,       // Update the health of an entity.
    kUpdateGameState = 0x157,    // Update the state of the game.
    kWorldInit = 0x158,          // Init the world content.
};

}  // namespace packet::type
