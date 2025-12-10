#include "Renderer.hpp"

#include <raylib.h>

#include <memory>

#include "logger/Logger.h"

namespace rteng::graphics {

Renderer::Renderer(int screenWidth, int screenHeight, const std::string& title, int fps)
{
    InitWindow(screenWidth, screenHeight, title.c_str());
    SetTargetFPS(fps);
}

Renderer::~Renderer()
{
    for (auto& texture : _textures) {
        if (!texture) {
            continue;
        }
        const ::Texture2D& tex = reinterpret_cast<::Texture2D&>(*texture);
        UnloadTexture(tex);
        texture.reset();
    }
    CloseWindow();
}

void Renderer::beginDrawing() { BeginDrawing(); }

void Renderer::endDrawing() { EndDrawing(); }

bool Renderer::windowShouldClose() { return WindowShouldClose(); }

void Renderer::clearBackground() { ClearBackground(::RAYWHITE); }

void Renderer::drawTexture(int textureId, const Rect& source, const Rect& dest, float rotation)
{
    if (!_textures[textureId]) {
        return;
    }

    ::Rectangle sourceRec = {source.x, source.y, source.width, source.height};
    ::Rectangle destRec = {dest.x, dest.y, dest.width, dest.height};
    ::Vector2 origin = {dest.width / 2, dest.height / 2};

    DrawTexturePro(reinterpret_cast<::Texture2D&>(_textures[textureId]), sourceRec, destRec, origin, rotation, ::WHITE);
}

void Renderer::drawRectangle(const Rect& rect, const Color& color)
{
    DrawRectangle((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, reinterpret_cast<const ::Color&>(color));
}

void Renderer::drawText(const std::string& text, int posX, int posY, int fontSize, const Color& color)
{
    DrawText(text.c_str(), posX, posY, fontSize, reinterpret_cast<const ::Color&>(color));
}

std::weak_ptr<Texture2D> Renderer::loadTexture(const std::string& filePath)
{
    ::Texture2D tex = LoadTexture(filePath.c_str());

    if (tex.id == 0) {
        LOG_ERR("Failed to load texture: {}", filePath);
        return {};
    }

    _textures.push_back(std::make_shared<Texture2D>(reinterpret_cast<Texture2D&>(tex)));
    return _textures.back();
}

}  // namespace rteng::graphics
