#include "app.hpp"

#include "handlers/handlers.hpp"
#include "logger/Thread.h"
#include "packets/server/spawn.hpp"
#include "systems/network.hpp"

namespace client {

App::App(const std::string& host,
         const short port)
    : _client(_context),
      _toolbox({components::Factory(components::GameComponents{}),
                rteng::GameEngine(components::GameComponents{}),
                {}})
{
    registerAllCallbacks();
    _client.connect(host, port);
    _ioThread = std::thread([this]() {
        logger::setThreadLabel("IoThread");
        _context.run();
    });
    _ioThread.detach();
    _isContextRunning = true;
}

void App::registerAllSystems()
{
    _toolbox.engine.getEcs()->registerSystem(
        std::make_unique<systems::Network>(_client, _networkService));
}

void App::registerAllCallbacks()
{
    _client.onConnect([]() { LOG_INFO("Connected."); });
    _client.onMessage(
        [](const rtnt::core::Packet& p) { LOG_DEBUG("Received a message (#{})", p.getId()); });
    _client.onDisconnect([]() { LOG_INFO("Disconnected."); });

    _client.getPacketDispatcher()
        .bind<packet::Spawn>(  // TODO: Push this function to a ConcurrentQueue;
            [this](const SessionPtr&, const packet::Spawn& p) {
                packet::handler::handleSpawn(p, _toolbox);
            });
}

App::~App() { stop(); }

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

void App::run() const
{
    utils::LoopTimer loopTimer(TPS);
    while (_isContextRunning) {
        _toolbox.engine.runOnce(0.016);  // Magic number for 60 fps
        loopTimer.waitForNextTick();
    }
}

}  // namespace client
