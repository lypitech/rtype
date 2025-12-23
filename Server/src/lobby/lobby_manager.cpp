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

void Manager::pushActionToLobby(const packet::server::SessionPtr& session,
                                const Callback& action)
{
    const auto it = _playerLookup.find(session);
    if (it != _playerLookup.end()) {
        Lobby* lobby = it->second;
        lobby->pushTask(action);
    } else {
        LOG_WARN("Session {} is not in any lobby.", session->getId());
    }
}

void Manager::joinRoom(const packet::server::SessionPtr& session,
                       const lobby::Id roomId)
{
    if (_lobbies.contains(roomId)) {
        _lobbies.at(roomId)->join(session);
        _playerLookup[session] = _lobbies.at(roomId).get();
    }
}

void Manager::leaveRoom(const packet::server::SessionPtr& session)
{
    const auto it = _playerLookup.find(session);
    if (it != _playerLookup.end()) {
        it->second->leave(session);
        _playerLookup.erase(it);
    }
}

}  // namespace lobby
