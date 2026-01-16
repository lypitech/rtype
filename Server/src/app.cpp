#include "app.hpp"

#include "enums/game_state.hpp"
#include "handlers/handlers.hpp"
#include "logger/Logger.h"
#include "logger/Thread.h"
#include "packets/client/join.hpp"
#include "packets/client/start.hpp"
#include "utils.hpp"

namespace server {

App::App(const unsigned short port,
         const std::string& config = "")
    : _server(_context,
              port),
      _lobbyManager(_outGoing,
                    config)
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

    packet::server::SendInterface sendInterface;
    while (true) {
        while (_outGoing.pop(sendInterface)) {
            for (const auto& session : sendInterface.first) {
                std::visit([&](auto&& p) { _server.sendTo(session, p); }, sendInterface.second);
            }
        }
        _server.update();
        loopTimer.waitForNextTick();
    }
}

void App::registerCallbacks()
{
    _server.getPacketDispatcher().bind<packet::UserInput>(
        [this](const SessionPtr& s, const packet::UserInput& u) {
            _lobbyManager.pushActionToLobby(
                s, [s, u](Lobby& lobby) { packet::handler::handleUserInput(s, lobby, u); });
        });
    _server.getPacketDispatcher().bind<packet::Join>(
        [this](
            const SessionPtr& s, const packet::Join& j) { _lobbyManager.joinRoom(s, j.room_id); });
    _server.getPacketDispatcher().bind<packet::Start>(
        [this](const SessionPtr& s, const packet::Start&) {
            _lobbyManager.pushActionToLobby(
                s, [](Lobby& lobby) { lobby.changeGameState(game::state::GameRunning); });
        });
}

}  // namespace server
