#pragma once
#include <asio/io_context.hpp>
#include <string>

#include "concurrent_queue.hpp"
#include "rteng.hpp"
#include "rtnt/core/client.hpp"

namespace client {

using Callback = std::function<void(rteng::GameEngine&)>;

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
    rteng::GameEngine _engine;
};

}  // namespace client
