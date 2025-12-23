#include "lobby.hpp"

#include <ranges>

#include "components/factory.hpp"
#include "components/position.hpp"
#include "components/type.hpp"
#include "enums/entity_types.hpp"
#include "logger/Thread.h"

Lobby::Lobby(const lobby::Id id,
             packet::server::OutGoingQueuePtr& outGoing)
    : _roomId(id),
      _outGoing(outGoing),
      _engine(components::GameComponents{}),
      _isRunning(false)
{
    LOG_INFO("Creating new lobby.");
}

lobby::Id Lobby::getRoomId() const { return _roomId; }

void Lobby::pushTask(const lobby::Callback& action) { _actionQueue.push(action); }

bool Lobby::hasJoined(const rtnt::core::session::Id sessionId) const
{
    return _players.contains(sessionId);
}

bool Lobby::join(const rtnt::core::session::Id sessionId)
{
    if (_players.contains(sessionId)) {
        LOG_WARN("Player already joined this lobby.");
        return false;
    }
    _players.try_emplace(sessionId, 0);
    if (_players.contains(sessionId)) {
        _actionQueue.push(
            [](rteng::GameEngine&
                   engine) {  // Create a new player entity on join (maybe do it otherwise)
                engine.registerEntity<components::Position, components::Type>(
                    nullptr, {10, 10}, {entity::Type::kPlayer});
            });
        return true;
    }
    LOG_WARN("Player couldn't join this lobby");
    return false;
}

void Lobby::leave(const rtnt::core::session::Id sessionId)
{
    if (_players.contains(sessionId)) {
        _players.erase(sessionId);
    } else {
        LOG_WARN("Player was not in this lobby");
    }
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

