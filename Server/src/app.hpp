#pragma once
#include <thread>

#include "lobby/lobby_manager.hpp"
#include "rtnt/core/server.hpp"

#define TPS 20

namespace server {

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
     * @param config The wave configuration file.
     */
    explicit App(unsigned short port,
                 const std::string& config);
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
