#include "rteng.hpp"

#include "Renderer.hpp"
// behaviour/component
#include "SparseSet.hpp"
#include "comp/Behaviour.hpp"
#include "logger/Thread.h"

namespace rteng {

GameEngine::GameEngine(std::string host, unsigned short port)
    : _client(std::make_unique<rtnt::core::Client>(_context)), _host(host), _port(port)
{
    _client->onMessage([](rtnt::core::Packet&) { LOG_INFO("Received message."); });
    _client->onConnect([]() { LOG_INFO("Successfully connected."); });
    _client->onDisconnect([]() { LOG_INFO("Disconnected from host."); });
}

GameEngine::GameEngine(unsigned short port)
    : _server(std::make_unique<rtnt::core::Server>(_context, port)), _port(port), _isClient(false)
{
    using SessionPtr = std::shared_ptr<rtnt::core::Session>;

    _server->onMessage([](SessionPtr, rtnt::core::Packet&) { LOG_INFO("Received message from cli."); });
    _server->onConnect([](SessionPtr) { LOG_INFO("Accepting new connection."); });
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
        _client->connect(_host, _port);
    } else {
        _server->start();
    }
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
    while (_isRunning && (!_isClient || !WindowShouldClose())) {
        // 1. Input (Input System)
        // 2. Update (Update System)
        // 3. ECS

        // Update MonoBehaviour instances (Start called once, then Update each frame)
        {
            auto& behaviourComponents = _ecs->getComponent<comp::Behaviour>();
            auto& behaviourSparseSet = dynamic_cast<rtecs::SparseSet<comp::Behaviour>&>(behaviourComponents);

            const float dt = GetFrameTime();
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
        _ecs->applyAllSystems();

        // 4. Render (Rendering System)
        if (_isClient) {
            BeginDrawing();
            ClearBackground(WHITE);
            _renderer.drawText("Hello R-Type Engine!", 190, 200, 20, LIGHTGRAY);

            EndDrawing();
        }

        // 5. Timer (Timer System)
    }
}

void GameEngine::registerSystems(std::vector<std::unique_ptr<rtecs::ASystem> > systems)
{
    for (auto& system : systems) {
        _ecs->registerSystem(std::move(system));
    }
}

}  // namespace rteng
