#pragma once

#include "lobby.hpp"
#include "rtnt/core/session.hpp"

namespace lobby {

class Manager
{
public:
    explicit Manager() = default;
    ~Manager();

    [[nodiscard]] bool joinRoom(rtnt::core::session::Id sessionId,
                                lobby::Id roomId = 0) const;
    void stopAll() const;
    void leaveRoom(rtnt::core::session::Id sessionId);
    void pushActionToLobby(rtnt::core::session::Id sessionId,
                           Callback action);
    Id createLobby();

private:
    std::unordered_map<Id, std::unique_ptr<Lobby>> _lobbies;
    std::unordered_map<rtnt::core::session::Id, Lobby*> _playerLookup;
};

}  // namespace lobby
