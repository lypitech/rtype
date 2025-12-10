#include "../include/rteng.hpp"  // La déclaration de la classe

#include "raylib.h"

using namespace rteng;

GameEngine::GameEngine(int screenWidth, int screenHeight,
                       const std::string& title) {
    InitWindow(screenWidth, screenHeight, title.c_str());
    SetTargetFPS(60);
    m_running = true;
}

void GameEngine::run() {
    while (!WindowShouldClose() && m_running) {
        // 1. Input (Input System)
        // 2. Update (Update System)
        // 3. ECS

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // 4. Render (Rendering System)

        DrawText("Hello R-Type Engine!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();

        // 5. Timer (Timer System)
    }

    // Nettoyage de Raylib
    CloseWindow();
}