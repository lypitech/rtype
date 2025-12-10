#include "Renderer.hpp"

#include "logger/Logger.h"
#include "raylib.h"

namespace rteng {
namespace graphics {

void RaylibRenderer::clearBackground() { ClearBackground(RAYWHITE); }

void RaylibRenderer::drawTexture(int textureId, const Rect& source, const Rect& dest, float rotation)
{
    if (m_textures.count(textureId)) {
        Rectangle sourceRec = {source.x, source.y, source.width, source.height};
        Rectangle destRec = {dest.x, dest.y, dest.width, dest.height};
        Vector2 origin = {dest.width / 2, dest.height / 2};

        DrawTexturePro(m_textures.at(textureId), sourceRec, destRec, origin, rotation, WHITE);
    }
}

void RaylibRenderer::drawRectangle(const Rect& rect, Color color)
{
    Color raylibColor = color;
    DrawRectangle((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, raylibColor);
}

void RaylibRenderer::drawText(const std::string& text, int posX, int posY, int fontSize, Color color)
{
    Color raylibColor = color;
    DrawText(text.c_str(), posX, posY, fontSize, raylibColor);
}

int RaylibRenderer::loadTexture(const std::string& filePath)
{
    Texture2D tex = LoadTexture(filePath.c_str());

    if (tex.id == 0) {
        LOG_ERR("Failed to load texture: {}", filePath);
        return 0;
    }

    int id = m_nextTextureId++;
    m_textures[id] = tex;
    return id;
}

void RaylibRenderer::unloadTexture(int textureId)
{
    if (m_textures.count(textureId)) {
        UnloadTexture(m_textures.at(textureId));
        m_textures.erase(textureId);
    }
}

}  // namespace graphics
}  // namespace rteng
