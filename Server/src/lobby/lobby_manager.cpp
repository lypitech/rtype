#include "lobby_manager.hpp"

#include <ranges>

namespace lobby {

Manager::Manager(packet::server::OutGoingQueuePtr& outGoing)
    : _outGoing(outGoing)
{
}

Id Manager::createLobby()
{
    static Id nbLobbies = 0;
    _lobbies.emplace(nbLobbies, std::make_unique<Lobby>(nbLobbies, _outGoing));
    _lobbies.at(nbLobbies)->start();
    return nbLobbies++;
}

Manager::~Manager() { stopAll(); }

void Manager::stopAll() const
{
    for (const auto& lobby : _lobbies | std::views::values) {
        lobby->stop();
    }
}

void Manager::pushActionToLobby(rtnt::core::session::Id sessionId,
                                Callback action)
{
    const auto it = _playerLookup.find(sessionId);
    if (it != _playerLookup.end()) {
        Lobby* lobby = it->second;
        lobby->pushTask(std::move(action));
    } else {
        LOG_WARN("Session {} is not in any lobby.", sessionId);
    }
}

bool Manager::joinRoom(const rtnt::core::session::Id sessionId,
                       const lobby::Id roomId) const
{
    if (_lobbies.contains(roomId)) {
        return _lobbies.at(roomId)->join(sessionId);
    }
    return false;
}

void Manager::leaveRoom(rtnt::core::session::Id sessionId)
{
    const auto it = _playerLookup.find(sessionId);
    if (it != _playerLookup.end()) {
        it->second->leave(sessionId);
    }
}

}  // namespace lobby
