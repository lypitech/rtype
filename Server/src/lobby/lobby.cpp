#include "lobby.hpp"

#include <ranges>

#include "app.hpp"
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

rtecs::OptionalRef<components::Position> Lobby::getPlayerPosition(
    const packet::server::SessionPtr& session)
{
    if (!_players.contains(session)) {
        return std::nullopt;
    }
    using Pos = components::Position;
    return dynamic_cast<rtecs::SparseSet<Pos>&>(_engine.getEcs()->getComponent<Pos>())
        .get(_players.at(session));
}

std::optional<rtecs::EntityID> Lobby::getPlayerId(const packet::server::SessionPtr& session) const
{
    if (!_players.contains(session)) {
        return std::nullopt;
    }
    return _players.at(session);
}

void Lobby::send(const packet::server::SessionPtr& session,
                 const packet::server::Variant& packet) const
{
    _outGoing.push({{session}, packet});
}

void Lobby::broadcast(const packet::server::Variant& packet) const
{
    _outGoing.push({getAllSessions(), packet});
}

rteng::GameEngine& Lobby::getEngine() { return _engine; }

void Lobby::pushTask(const lobby::Callback& action) { _actionQueue.push(action); }

void Lobby::join(const packet::server::SessionPtr& session)
{
    _actionQueue.push([this, session](Lobby&) {
        LOG_INFO("Joining lobby.");
        spawnEntity<components::Position, components::Type>(
            {10, 10}, {entity::Type::kPlayer}, session);
        packet::JoinAck j = {static_cast<uint32_t>(_players.at(session)), true};
        send(session, j);
    });
}

void Lobby::leave(const packet::server::SessionPtr& session)
{
    _actionQueue.push([this, session](Lobby&) {
        if (_players.contains(session)) {
            // TODO: destroy the entity and send a destroy packet to all other sessions;
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

void Lobby::start(const std::string& config)
{
    _levelDirector.load(config);
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
    using namespace std::chrono;
    double lag = 0;
    logger::setThreadLabel(("Lobby " + std::to_string(_roomId)).c_str());
    lobby::Callback callbackFunction;
    time_point lastTime = steady_clock::now();

    while (_isRunning) {
        auto currentTime = steady_clock::now();
        duration<double> elapsed = currentTime - lastTime;
        lastTime = currentTime;

        lag += elapsed.count();

        while (_actionQueue.pop(callbackFunction)) {
            callbackFunction(*this);
        }

        while (lag >= server::TIME_PER_TICK) {
            _engine.runOnce(server::TIME_PER_TICK);
            _levelDirector.update(server::TIME_PER_TICK, *this);
            lag -= server::TIME_PER_TICK;
        }

        if (lag < server::TIME_PER_TICK) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
