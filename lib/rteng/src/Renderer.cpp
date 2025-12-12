#include "Renderer.hpp"

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
        UnloadTexture(*texture);
        texture.reset();
    }
    CloseWindow();
}

void Renderer::drawTexture(int textureId, const Rect& source, const Rect& dest, float rotation)
{
    if (!_textures[textureId]) {
        return;
    }

    Rectangle sourceRec = {source.x, source.y, source.width, source.height};
    Rectangle destRec = {dest.x, dest.y, dest.width, dest.height};
    Vector2 origin = {dest.width / 2, dest.height / 2};

    DrawTexturePro(*_textures[textureId], sourceRec, destRec, origin, rotation, WHITE);
}

void Renderer::drawRectangle(const Rect& rect, const Color& color)
{
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
}

void Renderer::drawText(const std::string& text, int posX, int posY, int fontSize, const Color& color)
{
    DrawText(text.c_str(), posX, posY, fontSize, color);
}

std::weak_ptr<Texture2D> Renderer::loadTexture(const std::string& filePath)
{
    Texture2D tex = LoadTexture(filePath.c_str());

    if (tex.id == 0) {
        LOG_ERR("Failed to load texture: {}", filePath);
        return {};
    }

    _textures.push_back(std::make_shared<Texture2D>(tex));
    return _textures.back();
}

}  // namespace rteng::graphics
