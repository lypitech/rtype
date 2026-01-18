#include "lobby.hpp"

#include <ranges>

#include "app.hpp"
#include "components/factory.hpp"
#include "components/hitbox.hpp"
#include "components/position.hpp"
#include "components/score.hpp"
#include "components/type.hpp"
#include "enums/entity_types.hpp"
#include "enums/game_state.hpp"
#include "enums/player_state.hpp"
#include "logger/Thread.h"
#include "systems/apply_enemy_movement.hpp"
#include "systems/apply_movement.hpp"
#include "systems/broadcast_updated_movements.hpp"

Lobby::Lobby(const lobby::Id id,
             packet::server::OutGoingQueue& outGoing)
    : _roomId(id),
      _outGoing(outGoing),
      _engine(components::GameComponents{}),
      _isRunning(false),
      _levelDirector()
{
    registerAllSystems();
    LOG_INFO("Creating new lobby.");
    _engine.setGameState(game::state::GameWaiting);
}

void Lobby::registerAllSystems()
{
    _engine.registerSystem(std::make_shared<server::systems::ApplyMovement>());
    _engine.registerSystem(std::make_shared<server::systems::ApplyEnemyMovement>());
    _engine.registerSystem(std::make_shared<server::systems::BroadcastUpdatedMovements>(*this));
}

lobby::Id Lobby::getRoomId() const { return _roomId; }

rtecs::types::OptionalRef<components::Position> Lobby::getPlayerPosition(
    const packet::server::SessionPtr& session)
{
    if (!_players.contains(session)) {
        return std::nullopt;
    }
    return _engine.getEcs()->group<components::Position>().template getEntity<components::Position>(
        _players.at(session));
}

std::optional<rtecs::types::EntityID> Lobby::getPlayerId(
    const packet::server::SessionPtr& session) const
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

void Lobby::changeGameState(const uint64_t& gameState)
{
    _engine.setGameState(gameState);
    broadcast(packet::UpdateGameState{gameState});
}

std::vector<components::Position> Lobby::getPlayerPositions()
{
    std::vector<components::Position> positions;
    for (const auto& playerSess : _players | std::views::keys) {
        const auto& posOpt = getPlayerPosition(playerSess);
        if (posOpt) {
            positions.push_back(posOpt.value().get());
        }
    }
    return positions;
}

bool Lobby::hasPlayerAlive()
{
    if (_engine.getGameState() != game::state::GameRunning) {
        return true;
    }
    return std::ranges::any_of(_players | std::views::values, [&](const auto& playerId) {
        const auto& stateOpt = _engine.getEntityFromGroup<components::State>(playerId);
        return stateOpt && stateOpt->get().state == player::state::PlayerAlive;
    });
}

void Lobby::restart()
{
    _levelDirector.restart();
    for (const auto& playerId : _players | std::views::values) {
        _engine.getEntityFromGroup<components::State>(playerId).value().get().state =
            player::state::PlayerAlive;
    }
}

void Lobby::pushTask(const lobby::Callback& action) { _actionQueue.push(action); }

void Lobby::join(const packet::server::SessionPtr& session)
{
    _actionQueue.push([this, session](Lobby&) {
        if (_players.contains(session)) {
            return;
        }
        if (_engine.getGameState() < game::state::GameLobby) {
            changeGameState(game::state::GameLobby);
        }
        LOG_INFO("Joining lobby.");
        using namespace components;
        const rtecs::types::EntityID id =
            spawnEntity<Position, Velocity, Health, Type, Hitbox, Score, Collision, State>(
                {200, 300},
                {0, 0, 10, 10},
                {100, 100},
                {entity::Type::kPlayer},
                {true, 150, 75},
                {},
                {},
                {player::state::PlayerWaiting},
                session);
        packet::JoinAck j = {_players.at(session), _roomId, _engine.getGameState(), true};
        send(session, j);
        if (id != 0) {
            packet::WorldInit w;
            w.state = _engine.getGameState();
            for (const auto& entity : _engine.getEcs()->getAllEntities()) {
                const auto& [bitset, content] =
                    _engine.getEntityInfos(components::GameComponents{}, entity);
                w.bitsets.push_back(bitset);
                w.entities.push_back(content);
                w.ids.push_back(entity);
            }
            broadcast(w);
        }
    });
}

void Lobby::leave(const packet::server::SessionPtr& session)
{
    _actionQueue.push([this, session](Lobby&) {
        if (_players.contains(session)) {
            const rtecs::types::EntityID id = _players.at(session);
            _engine.destroyEntity(id);
            broadcast(packet::Destroy{id, 0});
            _players.erase(session);
            LOG_INFO("Player {} left lobby {}", session->getId(), _roomId);
            if (_players.empty()) {
                LOG_INFO("Lobby empty.", session->getId(), _roomId);
                for (const auto& entityId : _engine.clearEcs()) {
                    broadcast(packet::Destroy{entityId, 0});
                }
                _engine.setGameState(game::state::GameWaiting);
                if (_roomId != 0) {
                    _isRunning = false;
                }
            }
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
    _engine.setGameState(game::state::GameWaiting);
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
            if (!hasPlayerAlive()) {
                changeGameState(game::state::GameOver);
            }
            _engine.runOnce(server::TIME_PER_TICK);
            if (_engine.getGameState() == game::state::GameRunning) {
                _levelDirector.update(server::TIME_PER_TICK, *this);
            }
            lag -= server::TIME_PER_TICK;
        }

        if (lag < server::TIME_PER_TICK) {
            std::this_thread::sleep_for(milliseconds(10));
        }
    }
}
