#include "app.hpp"

#include "components/me.hpp"
#include "components/sprite.hpp"
#include "handlers/handlers.hpp"
#include "logger/Thread.h"
#include "packets/server/spawn.hpp"
#include "systems/IO.hpp"
#include "systems/network.hpp"
#include "systems/renderer.hpp"

namespace client {

App::App(const std::string& host,
         const short port)
    : _client(_context),
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
    _toolbox.engine.getEcs()->registerComponent<components::Sprite>();
    _toolbox.engine.getEcs()->registerComponent<components::Me>();
}

void App::registerAllSystems()
{
    _toolbox.engine.getEcs()->registerSystem(
        std::make_unique<systems::Network>(_client, _networkService));
    _toolbox.engine.getEcs()->registerSystem(std::make_unique<systems::Renderer>());
    _toolbox.engine.getEcs()->registerSystem(
        std::make_unique<systems::IO>(_toolbox.engine.getEcs(), _networkService));
}

void App::registerAllCallbacks()
{
    _client.onConnect([]() { LOG_INFO("Connected."); });
    _client.onMessage(
        [](const rtnt::core::Packet& p) { LOG_DEBUG("Received a message (#{})", p.getId()); });
    _client.onDisconnect([]() { LOG_INFO("Disconnected."); });

    _client.getPacketDispatcher().bind<packet::Spawn>(
        [this](const SessionPtr&, const packet::Spawn& p) {
            _actions.push([p](HandlerToolbox& tb) { packet::handler::handleSpawn(p, tb); });
        });
    _client.getPacketDispatcher().bind<packet::Destroy>(
        [this](const SessionPtr&, const packet::Destroy& p) {
            _actions.push([p](HandlerToolbox& tb) { packet::handler::handleDestroy(p, tb); });
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
    _client.getPacketDispatcher().bind<packet::JoinAck>(
        [this](const SessionPtr&, const packet::JoinAck& p) {
            _actions.push([p](HandlerToolbox& tb) { packet::handler::handleJoinAck(p, tb); });
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
    utils::LoopTimer loopTimer(TPS);
    while (_isContextRunning) {
        while (_actions.pop(action)) {
            action(_toolbox);
        }
        constexpr double dt = 1.0 / TPS;
        _toolbox.engine.runOnce(dt);
        loopTimer.waitForNextTick();
    }
}

}  // namespace client
