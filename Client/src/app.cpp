#include "app.hpp"

#include "components/animations.hpp"
#include "components/me.hpp"
#include "components/sound.hpp"
#include "components/sprite.hpp"
#include "components/target_pos.hpp"
#include "components/zindex.hpp"
#include "enums/game_state.hpp"
#include "enums/menu_state.hpp"
#include "handlers/handlers.hpp"
#include "logger/Thread.h"
#include "packets/server/spawn.hpp"
#include "systems/IO.hpp"
#include "systems/Menus.hpp"
#include "systems/interpolation.hpp"
#include "systems/network.hpp"
#include "systems/renderer.hpp"
#include "utils.hpp"

namespace client {

App::App(const std::string& host,
         const short port)
    : _shouldStop(false),
      _client(_context),
      _toolbox({components::Factory(components::GameComponents{}),
                rteng::GameEngine(components::GameComponents{}),
                {}})
{
    registerAllComponents();
    registerAllSystems();
    registerAllCallbacks();
    _client.connect(host, port);
    _ioThread = std::thread([this]() {
        logger::setThreadLabel("IoThread");
        _context.run();
    });
    _ioThread.detach();
    _isContextRunning = true;
}

App::~App() { stop(); }

void App::registerAllComponents()
{
    using namespace components;
    _toolbox.engine.getEcs()
        ->registerComponents<Animation, Me, components::Sound, Sprite, TargetPos, ZIndex>();
}

void App::registerAllSystems()
{
    _toolbox.engine.getEcs()->registerSystem(std::make_shared<systems::Interpolation>());
    _toolbox.engine.getEcs()->registerSystem(std::make_shared<systems::IO>(_networkService));
    _toolbox.engine.getEcs()->registerSystem(std::make_shared<systems::Renderer>(_shouldStop));
    _toolbox.engine.getEcs()->registerSystem(
        std::make_shared<systems::MenuRenderer>(_networkService, _toolbox.engine));
    _toolbox.engine.getEcs()->registerSystem(
        std::make_shared<systems::Network>(_client, _networkService));
}

void App::registerAllCallbacks()
{
    _client.onConnect([]() {
        LOG_INFO("Connected.");
        // packet::Join joinPacket;
        // joinPacket.username = "test";
        // joinPacket.room_id = 0;
        // _actions.push([this, joinPacket](HandlerToolbox&) { _client.send(joinPacket); });
    });
    _client.onMessage(
        [](const rtnt::core::Packet& p) { LOG_DEBUG("Received a message (#{})", p.getId()); });
    _client.onDisconnect([]() { LOG_INFO("Disconnected."); });

    _client.getPacketDispatcher().bind<packet::Destroy>(
        [this](const SessionPtr&, const packet::Destroy& p) {
            _actions.push([p](HandlerToolbox& tb) { packet::handler::handleDestroy(p, tb); });
        });
    _client.getPacketDispatcher().bind<packet::JoinAck>(
        [this](const SessionPtr&, const packet::JoinAck& p) {
            _actions.push([p](HandlerToolbox& tb) { packet::handler::handleJoinAck(p, tb); });
        });
    _client.getPacketDispatcher().bind<packet::Spawn>(
        [this](const SessionPtr&, const packet::Spawn& p) {
            _actions.push([p](HandlerToolbox& tb) { packet::handler::handleSpawn(p, tb); });
        });
    _client.getPacketDispatcher().bind<packet::UpdateEntityState>(
        [this](const SessionPtr&, const packet::UpdateEntityState& p) {
            _actions.push(
                [p](const HandlerToolbox& tb) { packet::handler::handleUpdateEntityState(p, tb); });
        });
    _client.getPacketDispatcher().bind<packet::UpdateGameState>(
        [this](const SessionPtr&, const packet::UpdateGameState& p) {
            _actions.push(
                [p](HandlerToolbox& tb) { packet::handler::handleUpdateGameState(p, tb); });
        });
    _client.getPacketDispatcher().bind<packet::UpdateHealth>(
        [this](const SessionPtr&, const packet::UpdateHealth& p) {
            _actions.push(
                [p](const HandlerToolbox& tb) { packet::handler::handleUpdateHealth(p, tb); });
        });
    _client.getPacketDispatcher().bind<packet::UpdatePosition>(
        [this](const SessionPtr&, const packet::UpdatePosition& p) {
            _actions.push(
                [p](HandlerToolbox& tb) { packet::handler::handleUpdatePosition(p, tb); });
        });
    _client.getPacketDispatcher().bind<packet::WorldInit>(
        [this](const SessionPtr&, const packet::WorldInit& p) {
            _actions.push([p](HandlerToolbox& tb) { packet::handler::handleWorldInit(p, tb); });
        });
}

void App::stop()
{
    if (!_isContextRunning) {
        return;
    }
    _context.stop();
    if (_ioThread.joinable()) {
        _ioThread.join();
    }
    _isContextRunning = false;
}

void App::run()
{
    Callback action;
    _toolbox.engine.setGameState(game::state::GameMenu);
    _toolbox.engine.setMenuState(menu::state::MenuHome);
    utils::LoopTimer loopTimer(TPS);
    while (_isContextRunning && !_shouldStop) {
        while (_actions.pop(action)) {
            action(_toolbox);
        }
        constexpr double dt = 1.0 / TPS;
        _toolbox.engine.runOnce(dt);
        loopTimer.waitForNextTick();
    }
}

}  // namespace client
