#include "app.hpp"

#include "components/all.hpp"
#include "logger/Thread.h"
#include "systems/network.hpp"

namespace client {

App::App(const std::string& host,
         const short port)
    : _client(_context),
      _engine(components::GameComponents{})
{
    _client.onConnect([]() { LOG_INFO("Connected."); });
    _client.onMessage(
        [](const rtnt::core::Packet& p) { LOG_INFO("Received a message (#{})", p.getId()); });
    _client.onDisconnect([]() { LOG_INFO("Disconnected."); });
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
    _engine.getEcs()->registerSystem(std::make_unique<systems::Network>(_client, _networkService));
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

void App::run()
{
    Callback func;
    while (_isContextRunning) {
        while (_queue.pop(func)) {
            func(_engine);
        }
        _engine.runOnce(0.16);  // Magic number for 60 fps
    }
}

}  // namespace client
