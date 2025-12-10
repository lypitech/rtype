#pragma once

#include <string>

namespace rteng {
class GameEngine {
   public:
    GameEngine(int screenWidth, int screenHeight, const std::string& title);

    void run();

    // Rendering System
    // Event System

   private:
    bool m_running = false;
};
}  // namespace rteng