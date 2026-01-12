#include "lobby.hpp"

#include <ranges>

#include "components/factory.hpp"
#include "components/position.hpp"
#include "components/type.hpp"
#include "enums/entity_types.hpp"
#include "logger/Thread.h"

Lobby::Lobby(const lobby::Id id,
             packet::server::OutGoingQueue& outGoing)
    : _roomId(id),
      _outGoing(outGoing),
      _engine(components::GameComponents{}),
      _isRunning(false)
{
    LOG_INFO("Creating new lobby.");
}

lobby::Id Lobby::getRoomId() const { return _roomId; }

void Lobby::pushTask(const lobby::Callback& action) { _actionQueue.push(action); }

void Lobby::join(const packet::server::SessionPtr& session)
{
    _actionQueue.push([this, session](rteng::GameEngine& engine) {
        LOG_INFO("Joining lobby.");
        rtecs::EntityID id = engine.registerEntity<components::Position, components::Type>(
            nullptr, {10, 10}, {entity::Type::kPlayer});
        _players[session] = id;
        const auto& infos = components::getEntityComponentsInfos(
            components::GameComponents{}, *engine.getEcs(), id);
        packet::Spawn p = {static_cast<uint32_t>(id), infos.first, infos.second};
        _outGoing.push({{getAllSessions()}, p});
    });
}

void Lobby::leave(const packet::server::SessionPtr& session)
{
    _actionQueue.push([this, session](rteng::GameEngine&) {
        if (_players.contains(session)) {
            // TODO: Send a destroy packet to all other sessions;
            _players.erase(session);
            LOG_INFO("Player left lobby {}", _roomId);
        } else {
            LOG_WARN("Session tried to leave lobby {} but was not in it.", _roomId);
        }
    });
}

void Lobby::stop()
{
    if (!_isRunning) {
        return;
    }
    LOG_INFO("Stopping lobby {}.", _roomId);
    _isRunning = false;
    if (_thread.joinable()) {
        _thread.join();
    }
}

void Lobby::start()
{
    _isRunning = true;
    _thread = std::thread(&Lobby::run, this);
    _thread.detach();
}

std::vector<packet::server::SessionPtr> Lobby::getAllSessions() const
{
    std::vector<packet::server::SessionPtr> sessions;

    sessions.reserve(_players.size());
    for (const auto& session : _players | std::views::keys) {
        sessions.push_back(session);
    }
    return sessions;
}

void Lobby::run()
{
    logger::setThreadLabel(("Lobby " + std::to_string(_roomId)).c_str());
    lobby::Callback callbackFunction;
    while (_isRunning) {
        while (_actionQueue.pop(callbackFunction)) {
            callbackFunction(_engine);
        }
        _engine.runOnce(0.16);
    }
}

