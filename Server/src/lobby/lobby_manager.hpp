#pragma once

#include "lobby.hpp"
#include "rtnt/core/session.hpp"

namespace lobby {

/**
 * @class Manager
 * @brief A simple lobby manager
 */
class Manager
{
public:
    explicit Manager(packet::server::OutGoingQueuePtr& outGoing);
    ~Manager();

    /**
     * @brief Tries to join a specific lobby.
     * @param session The pointer to the session trying to join.
     * @param roomId The id of the lobby to join.
     */
    void joinRoom(const packet::server::SessionPtr& session,
                  Id roomId = 0);
    /**
     * @brief Stops all lobbies.
     */
    void stopAll() const;

    /**
     * @brief Leaves the lobby corresponding to this sessionId.
     * @param session The pointer to the disconnecting session.
     */

    void leaveRoom(const packet::server::SessionPtr& session);
    /**
     * @brief Pushes an action to be performed by the lobby.
     * @param session The pointer to the session triggering the action.
     * @param action A function performing the triggered action.
     */

    void pushActionToLobby(const packet::server::SessionPtr& session,
                           const Callback& action);

    /**
     * @brief Creates a new lobby.
     * The creation of the lobbies is not automatic.
     * @return The id of the newly created lobby.
     */
    Id createLobby();

private:
    mutable std::mutex _mutex;
    packet::server::OutGoingQueuePtr& _outGoing;
    std::unordered_map<Id, std::unique_ptr<Lobby>> _lobbies;
    std::unordered_map<packet::server::SessionPtr, Lobby*> _playerLookup;
};

}  // namespace lobby
