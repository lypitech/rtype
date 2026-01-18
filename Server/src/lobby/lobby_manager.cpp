#include "lobby_manager.hpp"

#include <ranges>

#include "enums/game_state.hpp"

namespace lobby {

Manager::Manager(packet::server::OutGoingQueue& outGoing,
                 const std::string& config)
    : _outGoing(outGoing),
      _config(config)
{
}

Id Manager::createLobby()
{
    std::unique_lock lock(_mutex);
    static Id nbLobbies = 0;
    if (nbLobbies > 255) {
        return nbLobbies;
    }
    _lobbies.emplace(nbLobbies, std::make_unique<Lobby>(nbLobbies, _outGoing));
    _lobbies.at(nbLobbies)->start(_config);
    return nbLobbies++;
}

std::vector<Id> Manager::getLobbiesId(const uint32_t page) const
{
    std::vector<Id> roomIds;
    const uint32_t offset = page * 20;
    uint32_t i = 0;

    for (auto& lobby : _lobbies | std::views::keys) {
        if (i < offset) {
            i++;
            continue;
        }
        roomIds.push_back(lobby);
        if (roomIds.size() == 20) {
            break;
        }
    }
    return roomIds;
}

uint32_t Manager::getLobbiesMaxPage() const { return _lobbies.size() / 20 + 1; }

void Manager::update()
{
    for (size_t i = _lobbies.size() - 1; i > 0; --i) {
        const auto& lobby = _lobbies.at(i);
        if (!lobby->isRunning()) {
            lobby->stop();
            _lobbies.erase(i);
        }
    }
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
                       lobby::Id roomId)
{
    if (_playerLookup.contains(session)) {
        return;
    }
    if (roomId > 255) {
        const Id lobbyId = createLobby();
        if (lobbyId >= 255) {
            _outGoing.push({{session}, packet::JoinAck{0, 0, game::state::GameMenu, false}});
            return;
        }
        roomId = lobbyId;
    }
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
