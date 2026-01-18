#pragma once
#include <unordered_map>
#include <variant>

#include "components/factory.hpp"
#include "components/position.hpp"
#include "concurrent_queue.hpp"
#include "level_director/level_director.hpp"
#include "packets/server/destroy.hpp"
#include "packets/server/join_ack.hpp"
#include "packets/server/spawn.hpp"
#include "packets/server/update_entity_state.hpp"
#include "packets/server/update_game_state.hpp"
#include "packets/server/update_health.hpp"
#include "packets/server/update_position.hpp"
#include "packets/server/world_init.hpp"
#include "rteng.hpp"
#include "rtnt/core/session.hpp"

namespace packet::server {

using Variant = std::variant<Destroy,
                             JoinAck,
                             Spawn,
                             UpdateEntityState,
                             UpdateGameState,
                             UpdateHealth,
                             UpdatePosition,
                             WorldInit>;

using SessionPtr = std::shared_ptr<rtnt::core::Session>;

using SendInterface = std::pair<std::vector<SessionPtr>, Variant>;

using OutGoingQueue = utils::ConcurrentQueue<SendInterface>;

}  // namespace packet::server

class Lobby;

namespace lobby {

using Id = uint16_t;
using Callback = std::function<void(Lobby&)>;

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
     * @brief Register all the systems.
     */
    void registerAllSystems();

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
     * @param config The wave configuration file.
     */
    void start(const std::string& config);

    /**
     * @brief Stop this lobby.
     */
    void stop();

    /**
     * @brief Get the player id associated to the session.
     * @param session The session to retrieve the id from.
     * @return The entityId of the corresponding entity.
     */
    std::optional<rtecs::types::EntityID> getPlayerId(
        const packet::server::SessionPtr& session) const;

    /**
     * @brief Get the position of a player through its session pointer.
     * @param session A pointer to the session to retrieve the position from.
     * @return An optional reference to the position of the player connected t the session.
     */
    rtecs::types::OptionalRef<components::Position> getPlayerPosition(
        const packet::server::SessionPtr& session);

    /**
     * @brief Get a component of a player through its session pointer.
     * @param session A pointer to the session to retrieve the position from.
     * @return An optional reference to the position of the player connected t the session.
     */
    template <typename T>
    rtecs::types::OptionalRef<T> getPlayerComponent(const packet::server::SessionPtr& session)
    {
        if (!_players.contains(session)) {
            return std::nullopt;
        }
        return _engine.getEcs()->group<T>().template getEntity<T>(_players.at(session));
    }

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
     * @brief Create an entity and broadcasts it's creation.
     * @tparam Components The types of the components to add to the entity.
     * @param components The value of the components.
     * @param session The session triggering the creation (nullptr if none).
     */
    template <typename... Components>
    rtecs::types::EntityID spawnEntity(Components&&... components,
                                       const packet::server::SessionPtr& session = nullptr)
    {
        const rtecs::types::EntityID id = _engine.registerEntity<std::decay_t<Components>...>(
            nullptr, std::forward<Components>(components)...);
        if (session) {
            _players[session] = id;
        }
        const auto& [bitset, content] = _engine.getEntityInfos(components::GameComponents{}, id);
        packet::Spawn p = {id, bitset, content};
        broadcast(p);
        return id;
    }

    /**
     * @brief Remove an entity and broadcasts it's deletion.
     * @param id The entity's ID.
     * @param session The session triggering the deletion (nullptr if none).
     */
    rtecs::types::EntityID killEntity(rtecs::types::EntityID id,
                                      const packet::server::SessionPtr& session = nullptr);

    /**
     * @brief Getter for the gameEngine.
     * @return A reference to the used gameEngine.
     */
    rteng::GameEngine& getEngine();

    /**
     * @brief Changes the current gameState and updates the clients.
     * @param gameState The new state to put the game in.
     */
    void changeGameState(const uint64_t& gameState);

    std::vector<components::Position> getPlayerPositions();

    bool isRunning() const { return _isRunning; }

    bool hasPlayerAlive();

    void restart();

private:
    lobby::Id _roomId;
    utils::ConcurrentQueue<lobby::Callback> _actionQueue;
    packet::server::OutGoingQueue& _outGoing;
    rteng::GameEngine _engine;
    std::unordered_map<packet::server::SessionPtr, rtecs::types::EntityID> _players;
    std::atomic<bool> _isRunning;
    std::thread _thread;
    level::Director _levelDirector;

    void run();
    std::vector<packet::server::SessionPtr> getAllSessions() const;
};
