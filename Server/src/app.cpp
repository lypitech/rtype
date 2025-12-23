#include "app.hpp"

#include "logger/Logger.h"
#include "logger/Thread.h"
#include "utils.hpp"

namespace server {

App::App(const unsigned short port)
    : _server(_context,
              port)
{
    registerCallbacks();
    _server.onConnect(
        [](std::shared_ptr<rtnt::core::Session>) { LOG_INFO("Accepting new connection"); });
    _server.onDisconnect(
        [](std::shared_ptr<rtnt::core::Session> s) { LOG_INFO("Disconnected {}", s->getId()); });
    _server.onMessage([](std::shared_ptr<rtnt::core::Session> s, rtnt::core::Packet& p) {
        LOG_INFO("Client {} sent a packet of type {}", s->getId(), p.getId());
    });
    _lobbyManager.createLobby();
}

App::~App()
{
    LOG_INFO("Shutting down server.");
    _lobbyManager.stopAll();
    _context.stop();
    if (_ioThread.joinable()) {
        _ioThread.join();
    }
}

void App::start()
{
    _server.start();
    _ioThread = std::thread([this]() {
        logger::setThreadLabel("IoThread");
        _context.run();
    });
    _ioThread.detach();
    utils::LoopTimer loopTimer(TPS);

    while (true) {
        _server.update();
        loopTimer.waitForNextTick();
    }
}

void App::registerCallbacks()
{
    // Empty for now but register the packet callbacks here;
}

}  // namespace server
