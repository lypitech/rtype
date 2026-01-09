#pragma once
#include <unordered_map>
#include <variant>

#include "components/position.hpp"
#include "concurrent_queue.hpp"
#include "packets/server/destroy.hpp"
#include "packets/server/spawn.hpp"
#include "packets/server/update_position.hpp"
#include "rteng.hpp"
#include "rtnt/core/session.hpp"
#include "utils.hpp"

namespace packet::server {

using Variant =
    std::variant<Destroy, Spawn, UpdatePosition /* TODO: Add the remaining packet types */>;

using SessionPtr = std::shared_ptr<rtnt::core::Session>;

using SendInterface = std::pair<std::vector<SessionPtr>, Variant>;

using OutGoingQueue = utils::ConcurrentQueue<SendInterface>;

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
                   packet::server::OutGoingQueue& outGoing);

    /**
     * @brief Tries to join this lobby.
     * @param session The pointer to the session trying to join.
     * @return A boolean representing the status of the request.
     */
    void join(const packet::server::SessionPtr& session);

    /**
     * @return The id of this lobby.
     */
    lobby::Id getRoomId() const;

    /**
     * @brief Removes the @code SessionId@endcode from this lobby.
     * @param session The pointer to the session to remove.
     */
    void leave(const packet::server::SessionPtr& session);

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

    /**
     * @brief Get the player id associated to the session.
     * @param session The session to retrieve the id from.
     * @return The entityId of the corresponding entity.
     */
    std::optional<rtecs::EntityID> getPlayerId(const packet::server::SessionPtr& session) const;

    /**
     * @brief Get the position of a player through its session pointer.
     * @param session A pointer to the session to retrieve the position from.
     * @return An optional reference to the position of the player connected t the session.
     */
    rtecs::OptionalRef<components::Position> getPlayerPosition(
        const packet::server::SessionPtr& session);

    /**
     * @brief Send a packet to a specific session.
     * @param session The session to send the packet to.
     * @param packet The packet to send.
     */
    void send(const packet::server::SessionPtr& session,
              const packet::server::Variant& packet) const;

    /**
     * @brief Send a packet to all player present in the session.
     * @param packet The packet to send.
     */
    void broadcast(const packet::server::Variant& packet) const;

    /**
     * @brief Getter for the gameEngine.
     * @return A reference to the used gameEngine.
     */
    rteng::GameEngine& getEngine();

private:
    lobby::Id _roomId;
    utils::ConcurrentQueue<lobby::Callback> _actionQueue;
    packet::server::OutGoingQueue& _outGoing;
    rteng::GameEngine _engine;
    std::unordered_map<packet::server::SessionPtr, rtecs::EntityID> _players;
    std::atomic<bool> _isRunning;
    std::thread _thread;

    void run();
    std::vector<packet::server::SessionPtr> getAllSessions() const;
};
