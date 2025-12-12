#pragma once

#include <asio.hpp>
#include <memory>
#include <string>
#include <vector>

#include "ASystem.hpp"
#include "ECS.hpp"
#include "Renderer.hpp"

// components
#include "comp/IO.hpp"
#include "comp/Transform.hpp"
// components->monobehaviours
#include "MonoBehaviour.hpp"
#include "comp/Behaviour.hpp"
#include "rtnt/Core/client.hpp"
#include "rtnt/Core/server.hpp"

namespace rteng {

class GameEngine
{
public:
    explicit GameEngine(std::string host, unsigned short port);

    explicit GameEngine(unsigned short port);

    ~GameEngine();

    void init(int screenWidth, int screenHeight, const std::string& title, int fps = 60);

    void run();

    void stop() { _isRunning = false; }

    void onClientDisconnect(std::function<void()> callback);
    void onClientConnect(std::function<void()> callback);
    void onClientMessage(std::function<void(rtnt::core::Packet&)> callback);

    void onServerDisconnect(std::function<void(std::shared_ptr<rtnt::core::Session>)> callback);
    void onServerConnect(std::function<void(std::shared_ptr<rtnt::core::Session>)> callback);
    void onServerMessage(std::function<void(std::shared_ptr<rtnt::core::Session>, rtnt::core::Packet&)> callback);

    void registerSystems(std::vector<std::unique_ptr<rtecs::ASystem> > systems);
    template <typename T>
    void registerPacketHandler(std::function<void(const std::shared_ptr<rtnt::core::Session>&, const T&)> func)
    {
        if (_isClient) {
            return _client->getPacketDispatcher().bind(func);
        }
        return _server->getPacketDispatcher().bind(func);
    }
    template <typename component>
    void registerComponent()
    {
        _ecs->registerComponent<component>();
    }

    template <typename... component>
    void registerEntity(const std::shared_ptr<rteng::behaviour::MonoBehaviour>& mono_behaviour = nullptr)
    {
        const rtecs::EntityID entityId = _ecs->registerEntity<component...>();

        if (!_ecs->hasEntityComponent<comp::Behaviour>(entityId)) {
            return;
        }
        auto& behaviourComponents = _ecs->getComponent<comp::Behaviour>();
        auto& behaviourSparseSet = dynamic_cast<rtecs::SparseSet<comp::Behaviour>&>(behaviourComponents);

        comp::Behaviour behaviourComp;
        behaviourComp.instance = mono_behaviour;
        behaviourComp.started = false;

        behaviourSparseSet.put(entityId, behaviourComp);
    }

    // Rendering System
    // Event System

private:
    graphics::Renderer _renderer;
    std::unique_ptr<rtecs::ECS> _ecs = rtecs::ECS::createWithComponents<comp::Transform, comp::IO, comp::Behaviour>();
    asio::io_context _context;
    std::unique_ptr<rtnt::core::Client> _client;
    std::unique_ptr<rtnt::core::Server> _server;
    std::string _host{"localhost"};
    std::unique_ptr<std::thread> _ioThread;
    unsigned short _port;
    bool _isClient{true};
    bool _isInit{false};
    bool _isRunning = false;

    void runContext();
};

}  // namespace rteng
