#pragma once
#include <unordered_map>

#include "rteng.hpp"
#include "rtnt/core/session.hpp"

namespace comp {

struct Dummy
{
    int k;
};

}  // namespace comp

#define ALL_COMPONENTS comp::Dummy

namespace lobby {

using Id = uint32_t;
using Callback = std::function<void()>;

}  // namespace lobby

class Lobby
{
public:
    explicit Lobby(lobby::Id id);

    bool join(rtnt::core::session::Id sessionId);
    lobby::Id getRoomId() const;
    void leave(rtnt::core::session::Id sessionId);
    bool hasJoined(rtnt::core::session::Id sessionId) const;
    void pushTask(lobby::Callback action);
    void start();
    void stop();

private:
    lobby::Id _roomId;
    std::queue<lobby::Callback> _actionQueue;
    rteng::GameEngine _engine;
    std::unordered_map<rtnt::core::session::Id, size_t> _players;
    std::atomic<bool> _isRunning;
    std::thread _thread;

    void run();
};
