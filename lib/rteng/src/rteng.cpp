#include "rteng.hpp"

#include "Renderer.hpp"

namespace rteng {

GameEngine::GameEngine(int screenWidth, int screenHeight, const std::string &title, int fps)
    : _renderer(screenWidth, screenHeight, title, fps)
{
    _isRunning = true;
}

void GameEngine::run()
{
    while (!_renderer.windowShouldClose() && _isRunning) {
        // 1. Input (Input System)
        // 2. Update (Update System)
        // 3. ECS

        _renderer.beginDrawing();
        _renderer.clearBackground();

        _ecs->applyAllSystems();

        // 4. Render (Rendering System)

        _renderer.drawText("Hello R-Type Engine!", 190, 200, 20, graphics::LIGHTGRAY);

        _renderer.endDrawing();
        // 5. Timer (Timer System)
    }
}

}  // namespace rteng
