#pragma once
#include <string>
#include <thread>

#include "lobby/lobby_manager.hpp"
#include "rtnt/core/server.hpp"

#define TPS 20

namespace server {

class App
{
public:
    explicit App(unsigned short port);
    ~App();

    [[noreturn]] void start();

private:
    asio::io_context _context;
    rtnt::core::Server _server;
    std::thread _ioThread;
    lobby::Manager _lobbyManager;

    void registerCallbacks();
};

}  // namespace server
