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
    kJoin = 0x128,  ///< Ask to join a game.
    kStart,         ///< Ask to start a game.
    kUserInput,     ///< Send the bitmask of the input.
};

/**
 * @class Server
 *
 * @brief The id of the packet sent by the server.
 */
enum class Server : uint16_t
{
    kJoinAck = 0x150,    ///< Notify the status of a join request.
    kPlayerJoin,         ///< Notify that a player joined the lobby.
    kSpawn,              ///< Notify of an entity spawn.
    kDestroy,            ///< Notify of an entity destroying.
    kUpdateEntityState,  ///< Update the state of an entity.
    kUpdatePosition,     ///< Update the position of an entity.
    kUpdateHealth,       ///< Update the health of an entity.
    kUpdateGameState,    ///< Update the state of the game.
    kWorldInit,          ///< Init the world content.
};

}  // namespace packet::type
