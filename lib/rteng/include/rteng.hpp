#pragma once

#include <string>

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
    bool _isRunning = false;
};
}  // namespace rteng
