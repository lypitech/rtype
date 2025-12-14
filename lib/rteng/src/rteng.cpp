#include "rteng.hpp"

#include <memory.h>

#include "Renderer.hpp"
// behaviour/component
#include "SparseSet.hpp"
#include "comp/Behaviour.hpp"
#include "comp/Sprite.hpp"
#include "logger/Thread.h"
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
    using SessionPtr = std::shared_ptr<rtnt::core::Session>;

    if (_isClient) {
        _client->connect(_host, _port);
        registerPacketHandler<packet::Spawn>([this](const SessionPtr&, const packet::Spawn& packet) {
            const rtecs::DynamicBitSet bitset(packet.bitmask);

            if (_serverToClient.contains(packet.id)) {
                LOG_TRACE_R3("Entity has already been created, ignoring...");
                return;
            }
            const rtecs::EntityID real = _ecs->registerEntity(bitset);
            _serverToClient.emplace(packet.id, real);
            _factory.apply(*_ecs, real, bitset, packet.content);
        });
        const auto bitset = _ecs->getComponentsBitSet<comp::Rectangle, comp::Position>();
        _ecs->registerSystem(std::make_unique<sys::Rectangle>(bitset));
    } else {
        _server->start();
    }
    const auto bitset = _ecs->getComponentsBitSet<comp::Sprite>();
    _ecs->registerSystem(std::make_unique<sys::Sprite>(bitset));
    _ecs->registerSystem(std::make_unique<sys::Animation>(bitset));
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
    bool firstRun = true;
    while (_isRunning && (!_isClient || !WindowShouldClose())) {
        // 1. Input (Input System)
        // 2. Update (Update System)
        // 3. ECS

        if (firstRun) {  // Create a single entity.
            registerEntity<comp::Sprite>(
                nullptr, {
                             .shown = true,
                             .texture = LoadTexture("../../img.png"),  // Le client se trouve dans {build_dir}/client
                             .position = {0, 0},
                             .scale = 1.0f,
                             .color = {250, 252, 252, 255},
                         });
            firstRun = false;
        }

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
