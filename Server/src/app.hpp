#pragma once

#include <thread>

#include "lobby/lobby_manager.hpp"
#include "packets/client/join.hpp"
#include "rtnt/core/server.hpp"

namespace server {

static constexpr short TPS = 20;
static constexpr double TIME_PER_TICK = 1.0 / TPS;

/**
 * @class App
 * @brief A server application containing a lobby manager.
 */
class App
{
public:
    /**
     * @brief Creates a server listening to specified port.
     * @param port The port to listen to.
     */
    explicit App(unsigned short port);
    ~App();

    /**
     * @brief Starts the server and updates it periodically.
     */
    [[noreturn]] void start();

private:
    packet::server::OutGoingQueue _outGoing;
    asio::io_context _context;
    rtnt::core::Server _server;
    std::thread _ioThread;
    lobby::Manager _lobbyManager;

    void registerCallbacks();
};

}  // namespace server
