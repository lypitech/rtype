#include "lobby_manager.hpp"

#include <ranges>

namespace lobby {

Manager::Manager(packet::server::OutGoingQueue& outGoing)
    : _outGoing(outGoing)
{
}

Id Manager::createLobby()
{
    std::unique_lock lock(_mutex);
    static Id nbLobbies = 0;
    _lobbies.emplace(nbLobbies, std::make_unique<Lobby>(nbLobbies, _outGoing));
    _lobbies.at(nbLobbies)->start();
    return nbLobbies++;
}

Manager::~Manager() { stopAll(); }

void Manager::stopAll() const
{
    std::unique_lock lock(_mutex);
    for (const auto& lobby : _lobbies | std::views::values) {
        lobby->stop();
    }
}

void Manager::pushActionToLobby(const packet::server::SessionPtr& session,
                                const Callback& action)
{
    std::unique_lock lock(_mutex);
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
    std::unique_lock lock(_mutex);
    if (_lobbies.contains(roomId)) {
        _lobbies.at(roomId)->join(session);
        _playerLookup[session] = _lobbies.at(roomId).get();
    }
}

void Manager::leaveRoom(const packet::server::SessionPtr& session)
{
    std::unique_lock lock(_mutex);
    const auto it = _playerLookup.find(session);
    if (it != _playerLookup.end()) {
        it->second->leave(session);
        _playerLookup.erase(it);
    }
}

}  // namespace lobby
