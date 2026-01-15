#pragma once

#include <chrono>
#include <thread>
#include <vector>

#include "metrics.hpp"
#include "rtnt/core/client.hpp"
#include "rtnt/core/server.hpp"

namespace rtnt::stat {

using namespace std::chrono;

class Recorder final
{
public:
    explicit Recorder(core::Server& server)
        : _peer(server),
          _server(&server)
    {
    }

    explicit Recorder(core::Client& client)
        : _peer(client),
          _client(&client)
    {
    }

    ~Recorder() { stop(); }

    void start(milliseconds interval = seconds(1));
    void stop();
    void exportToCsv(const std::string& filename) const;

private:
    core::Peer& _peer;

    core::Server* _server = nullptr;
    core::Client* _client = nullptr;

    std::vector<SystemSnapshot> _history;
    mutable std::mutex _historyMutex;

    std::atomic<bool> _isRunning = false;
    std::thread _workerThread;
    std::condition_variable _cv;
    std::mutex _cvMutex;

    void _workerLoop(milliseconds interval);
    SystemSnapshot _takeSnapshot();
};

}  // namespace rtnt::stat
