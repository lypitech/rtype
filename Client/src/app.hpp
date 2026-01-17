#pragma once
#include <asio/io_context.hpp>
#include <string>

#include "components/factory.hpp"
#include "concurrent_queue.hpp"
#include "handlers/handlers.hpp"
#include "rteng.hpp"
#include "rtnt/core/client.hpp"
#include "systems/network.hpp"

constexpr size_t TPS = 60;

namespace client {

struct Lobby
{
    std::vector<uint32_t> roomIds;
    uint32_t page;
    uint32_t maxPage;
};

/**
 * @brief A toolbox for the packet handler functions that contain everything they might need.
 */
struct HandlerToolbox
{
    components::Factory componentFactory;  ///< A component factory to create an entity.
    rteng::GameEngine engine;              ///< The game engine instance.
    std::unordered_map<rtecs::types::EntityID, rtecs::types::EntityID>
        serverToClient;  ///< A map binding the entity id that the server uses and the real one
};

using Callback = std::function<void(HandlerToolbox&)>;

/**
 * @class App
 * @brief The main client application.
 */
class App final
{
public:
    /**
     * @brief Creates a @code rtnt::Client@endcode connecting to host:port
     * DNS deduction is not implemented yet.
     * @param host The host to connect to.
     * @param port The port to connect to on the server.
     */
    App(const std::string& host,
        short port);

    ~App();

    /**
     * @brief Run the game including network sync.
     */
    void run();

    /**
     * @brief Stop the app.
     */
    void stop();

private:
    bool _isContextRunning;
    bool _shouldStop;
    utils::ConcurrentQueue<Callback> _actions;
    Lobby _lobbies;
    std::thread _ioThread;
    asio::io_context _context;
    rtnt::core::Client _client;
    HandlerToolbox _toolbox;
    service::Network _networkService;

    void registerAllSystems();
    void registerAllComponents();
    void registerAllCallbacks();
};

}  // namespace client
