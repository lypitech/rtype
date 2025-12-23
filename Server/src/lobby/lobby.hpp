#pragma once
#include <unordered_map>

#include "concurrent_queue.hpp"
#include "packets/server/destroy.hpp"
#include "packets/server/spawn.hpp"
#include "rteng.hpp"
#include "rtnt/core/session.hpp"
#include "utils.hpp"

namespace packet::server {

using Variant = std::variant<Destroy, Spawn /* TODO: Add the remaining packet types */>;

using SessionPtr = std::shared_ptr<rtnt::core::Session>;

using SendInterface = std::pair<std::vector<SessionPtr>, Variant>;

using OutGoingQueuePtr = std::unique_ptr<utils::ConcurrentQueue<SendInterface>>;

}  // namespace packet::server

namespace lobby {

using Id = uint32_t;
using Callback = std::function<void(rteng::GameEngine&)>;

}  // namespace lobby

/**
 * @class Lobby
 * @brief Encapsulates a gameEngine instance and it's networking interface
 */
class Lobby
{
public:
    /**
     * @brief Creates a lobby with the specified @code id@endcode.
     * @param id an uint32(lobby::Id) that represents the id of the lobby.
     * @param outGoing queue for outgoing packets.
     *
     * Note that the uniqueness of the ID depends on the user providing a distinct value.
     */
    explicit Lobby(lobby::Id id,
                   packet::server::OutGoingQueuePtr& outGoing);

    /**
     * @brief Tries to join this lobby.
     * @param sessionId The id of the session trying to join.
     * @return A boolean representing the status of the request.
     */
    bool join(rtnt::core::session::Id sessionId);

    /**
     * @return The id of this lobby.
     */
    lobby::Id getRoomId() const;

    /**
     * @brief Removes the @code SessionId@endcode from this lobby.
     * @param sessionId The id of the session to remove.
     */
    void leave(rtnt::core::session::Id sessionId);
    /**
     * @param sessionId The researched id.
     * @return Whether this lobby contains this @code sessionId@endcode.
     */
    bool hasJoined(rtnt::core::session::Id sessionId) const;

    /**
     * @brief Pushes a task to be made inside the running thread.
     * This function is thread-safe.
     * @param action A function performing the required action.
     */
    void pushTask(const lobby::Callback& action);

    /**
     * @brief Start this lobby.
     */
    void start();

    /**
     * @brief Stop this lobby.
     */
    void stop();

private:
    lobby::Id _roomId;
    utils::ConcurrentQueue<lobby::Callback> _actionQueue;
    packet::server::OutGoingQueuePtr& _outGoing;
    rteng::GameEngine _engine;
    std::unordered_map<rtnt::core::session::Id, rtecs::EntityID> _players;
    std::atomic<bool> _isRunning;
    std::thread _thread;

    void run();
};
