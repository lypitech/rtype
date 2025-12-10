#pragma once

#include <string>

#include "ECS.hpp"
#include "Renderer.hpp"

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
    rtecs::ECS _ecs;
    bool _isRunning = false;
};
}  // namespace rteng
