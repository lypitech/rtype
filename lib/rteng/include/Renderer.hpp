#pragma once

#include <raylib.h>

#include <memory>
#include <string>
#include <vector>

namespace rteng::graphics {

struct Rect
{
    float x;
    float y;
    float width;
    float height;
};

class Renderer
{
public:
    Renderer() = default;

    void init(int screenWidth, int screenHeight, const std::string& title, int fps = 60);
    ~Renderer();

    void drawTexture(int textureId, const Rect& source, const Rect& dest, float rotation);

    void drawRectangle(const Rect& rect, const Color& color);

    void drawText(const std::string& text, int posX, int posY, int fontSize, const Color& color);

    std::weak_ptr<Texture2D> loadTexture(const std::string& filePath);

private:
    std::vector<std::shared_ptr<Texture2D>> _textures;
    bool _isInit{false};
};

}  // namespace rteng::graphics

