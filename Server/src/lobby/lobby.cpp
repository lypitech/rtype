#include "lobby.hpp"

#include "logger/Thread.h"

Lobby::Lobby(const lobby::Id id)
    : _roomId(id),
      _engine(rteng::ComponentsList<ALL_COMPONENTS>{}),
      _isRunning(false)
{
    LOG_INFO("Creating new lobby.");
}

lobby::Id Lobby::getRoomId() const { return _roomId; }

void Lobby::pushTask(lobby::Callback action) { _actionQueue.push(std::move(action)); }

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
    while (_isRunning) {
        _engine.runOnce(0.16);
    }
}

