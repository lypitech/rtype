#include "rteng.hpp"

#include <memory.h>

#include "Renderer.hpp"
// behaviour/component
#include "SparseSet.hpp"
#include "comp/Behaviour.hpp"
#include "comp/Sprite.hpp"
#include "handlers/client_side/handlers.hpp"
#include "handlers/server/handlers.hpp"
#include "logger/Thread.h"
#include "packets/client/user_input.hpp"
#include "sys/IO.hpp"
#include "sys/Sprite.hpp"
#include "sys/rectangle.hpp"

namespace rteng {

GameEngine* kInstance = nullptr;

GameEngine& GameEngine::getInstance()
{
    if (kInstance == nullptr) {
        LOG_FATAL("GameEngine in not instantiated yet. ");
        std::exit(1);
    }
    return *kInstance;
}

GameEngine::GameEngine(std::string host, unsigned short port)
    : _client(std::make_unique<rtnt::core::Client>(_context)), _host(host), _port(port)
{
    kInstance = this;
    _client->onMessage([](rtnt::core::Packet& packet) { LOG_INFO("Received message (#{}).", packet.getId()); });
    _client->onConnect([]() { LOG_INFO("Successfully connected."); });
    _client->onDisconnect([]() { LOG_INFO("Disconnected from host."); });
}

GameEngine::GameEngine(unsigned short port)
    : _server(std::make_unique<rtnt::core::Server>(_context, port)), _port(port), _isClient(false)
{
    kInstance = this;
    using SessionPtr = std::shared_ptr<rtnt::core::Session>;

    _server->onMessage([](SessionPtr, rtnt::core::Packet&) { LOG_INFO("Received message from cli."); });
    _server->onConnect([this](SessionPtr s) {
        LOG_INFO("Accepting new connection.");
        auto id = registerEntity<comp::IO, comp::Position, comp::Rectangle>(nullptr, {}, {25, 25},
                                                                            {true, 150, 150, BLACK, RED});
        _clientToServer.emplace(s->getId(), id);
    });
    _server->onDisconnect([](SessionPtr) { LOG_INFO("Client disconnected."); });
}

GameEngine::~GameEngine()
{
    _context.stop();
    if (_ioThread->joinable()) {
        _ioThread->join();
    }
}

void GameEngine::onClientMessage(std::function<void(rtnt::core::Packet&)> callback)
{
    if (_isClient) {
        _client->onMessage(callback);
    }
}

void GameEngine::onClientConnect(const std::function<void()> callback)
{
    if (_isClient) {
        _client->onConnect(callback);
    }
}

void GameEngine::onClientDisconnect(std::function<void()> callback)
{
    if (_isClient) {
        _client->onDisconnect(callback);
    }
}

void GameEngine::onServerMessage(std::function<void(std::shared_ptr<rtnt::core::Session>, rtnt::core::Packet&)> func)
{
    if (!_isClient) {
        _server->onMessage(func);
    }
}

void GameEngine::onServerConnect(std::function<void(std::shared_ptr<rtnt::core::Session>)> func)
{
    if (!_isClient) {
        _server->onConnect(func);
    }
}

void GameEngine::onServerDisconnect(std::function<void(std::shared_ptr<rtnt::core::Session>)> func)
{
    if (!_isClient) {
        _server->onDisconnect(func);
    }
}

void GameEngine::init()
{
    if (_isClient) {
        registerPacketHandler<packet::Spawn>(client_side::handlers::handleSpawn);
        registerPacketHandler<packet::UpdatePosition>(client_side::handlers::handleUpdatePosition);
        _tps = 60;
        _client->connect(_host, _port);
    } else {
        registerPacketHandler<packet::UserInput>(server_side::handlers::handleUserInput);
        _tps = 1;
        _server->start();
    }
    _ecs->registerSystem(std::make_unique<sys::Sprite>(_ecs));
    _ecs->registerSystem(std::make_unique<sys::Animation>(_ecs));
    _ecs->registerSystem(std::make_unique<sys::Rectangle>(_ecs));
    _ecs->registerSystem(std::make_unique<sys::IO>(_ecs));
    _isInit = true;
    _isRunning = true;
}

void GameEngine::init(int screenWidth, int screenHeight, const std::string& title, int fps)
{
    init();
    _renderer.init(screenWidth, screenHeight, title, fps);
}

void GameEngine::runContext()
{
    logger::setThreadLabel("IoThread");
    LOG_DEBUG("Running context");
    _context.run();
}

void GameEngine::run()
{
    if (!_isInit) {
        LOG_ERR("GameEngine is not initialized");
        return;
    }
    _ioThread = std::make_unique<std::thread>(std::bind(&GameEngine::runContext, this));
    const auto timePerFrame = std::chrono::nanoseconds(1000000000 / _tps);
    auto nextUpdate = std::chrono::steady_clock::now();
    while (_isRunning && (!_isClient || !WindowShouldClose())) {
        // 1. Input (Input System)
        // 2. Update (Update System)
        // 3. ECS

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // pas niquer le ssd/proc
        nextUpdate += timePerFrame;

        // Update MonoBehaviour instances (Start called once, then Update each frame)
        {
            const float dt = GetFrameTime();
            auto& behaviourComponents = _ecs->getComponent<comp::Behaviour>();
            auto& behaviourSparseSet = dynamic_cast<rtecs::SparseSet<comp::Behaviour>&>(behaviourComponents);

            for (auto& [instance, started] : behaviourSparseSet.getAll()) {
                if (!instance) {
                    continue;
                }
                if (!started) {
                    instance->Start();
                    started = true;
                }
                instance->Update(dt);
            }
        }
        if (_isClient) {
            BeginDrawing();
            ClearBackground(WHITE);
        }
        _ecs->applyAllSystems();

        // 4. Render (Rendering System)
        if (_isClient) {
            _renderer.drawText("Hello R-Type Engine!", 190, 200, 20, LIGHTGRAY);

            EndDrawing();
        }

        // 5. Timer (Timer System)
        if (!_isClient) {
            _server->update();
        }
        std::this_thread::sleep_until(nextUpdate);
    }
}

void GameEngine::registerSystems(std::vector<std::unique_ptr<rtecs::ASystem> > systems)
{
    for (auto& system : systems) {
        _ecs->registerSystem(std::move(system));
    }
}

}  // namespace rteng
