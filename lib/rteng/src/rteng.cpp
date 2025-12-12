#include "rteng.hpp"

#include "Renderer.hpp"
// behaviour/component
#include "SparseSet.hpp"
#include "comp/Behaviour.hpp"

namespace rteng {

GameEngine::GameEngine(int screenWidth, int screenHeight, const std::string& title, int fps)
    : _renderer(screenWidth, screenHeight, title, fps)
{
    _isRunning = true;
}

void GameEngine::run()
{
    while (!WindowShouldClose() && _isRunning) {
        // 1. Input (Input System)
        // 2. Update (Update System)
        // 3. ECS

        BeginDrawing();
        ClearBackground(WHITE);

        // Update MonoBehaviour instances (Start called once, then Update each frame)
        {
            auto& behaviourComponents = _ecs->getComponent<comp::Behaviour>();
            rtecs::SparseSet<comp::Behaviour>& behaviourSparseSet =
                dynamic_cast<rtecs::SparseSet<comp::Behaviour>&>(behaviourComponents);

            const float dt = GetFrameTime();
            for (auto& b : behaviourSparseSet.getAll()) {
                if (!b.instance) {
                    continue;
                }
                if (!b.started) {
                    b.instance->Start();
                    b.started = true;
                }
                b.instance->Update(dt);
            }
        }
        _ecs->applyAllSystems();

        // 4. Render (Rendering System)

        _renderer.drawText("Hello R-Type Engine!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        // 5. Timer (Timer System)
    }
}

}  // namespace rteng
