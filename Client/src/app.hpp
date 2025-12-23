#pragma once
#include <asio/io_context.hpp>
#include <string>

#include "components/factory.hpp"
#include "concurrent_queue.hpp"
#include "rteng.hpp"
#include "rtnt/core/client.hpp"
#include "systems/network.hpp"

namespace client {

using Callback = std::function<void(rteng::GameEngine&)>;

struct HandlerToolbox
{
    components::Factory componentFactory;
    rteng::GameEngine engine;
    std::unordered_map<rtecs::EntityID, rtecs::EntityID> serverToClient;
};

class App
{
public:
    App(const std::string& host,
        short port);

    ~App();

    void run();
    void stop();

private:
    utils::ConcurrentQueue<Callback> _queue;
    bool _isContextRunning;
    std::thread _ioThread;
    asio::io_context _context;
    rtnt::core::Client _client;
    HandlerToolbox _toolbox;
    service::Network _networkService;

    void registerAllSystems();
    void registerAllCallbacks();
};

}  // namespace client
