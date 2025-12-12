#pragma once

#include <memory>
#include <string>

#include "ECS.hpp"
#include "Renderer.hpp"

// components
#include "comp/IO.hpp"
#include "comp/Transform.hpp"
// components->monobehaviours
#include "comp/Behaviour.hpp"

namespace rteng {
class GameEngine
{
public:
    GameEngine(int screenWidth, int screenHeight, const std::string& title, int fps = 60);

    void run();

    // Rendering System
    // Event System

private:
    graphics::Renderer _renderer;
    std::unique_ptr<rtecs::ECS> _ecs = rtecs::ECS::createWithComponents<comp::Transform, comp::IO, comp::Behaviour>();
    bool _isRunning = false;
};
}  // namespace rteng
