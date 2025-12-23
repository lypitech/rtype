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
     * @param sessionId The id of the session trying to join.
     * @param roomId The id of the lobby to join.
     * @return Whether the request is successful.
     */
    [[nodiscard]] bool joinRoom(rtnt::core::session::Id sessionId,
                                Id roomId = 0) const;
    /**
     * @brief Stops all lobbies.
     */
    void stopAll() const;
    /**
     * @brief Leaves the lobby corresponding to this sessionId.
     * @param sessionId The id of the disconnecting session.
     */
    void leaveRoom(rtnt::core::session::Id sessionId);
    /**
     * @brief Pushes an action to be performed by the lobby.
     * @param sessionId The id of the session triggering the action.
     * @param action A function performing the triggered action.
     */
    void pushActionToLobby(rtnt::core::session::Id sessionId,
                           Callback action);
    /**
     * @brief Creates a new lobby.
     * The creation of the lobbies is not automatic.
     * @return The id of the newly created lobby.
     */
    Id createLobby();

private:
    packet::server::OutGoingQueuePtr& _outGoing;
    std::unordered_map<Id, std::unique_ptr<Lobby>> _lobbies;
    std::unordered_map<rtnt::core::session::Id, Lobby*> _playerLookup;
};

}  // namespace lobby
