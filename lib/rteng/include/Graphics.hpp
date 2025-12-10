#pragma once

#include <string>

namespace rteng {
namespace graphics {

struct Rect {
    float x, y, width, height;
};

class IRenderer {
   public:
    virtual ~IRenderer() = default;

    virtual void beginDrawing() = 0;
    virtual void endDrawing() = 0;
    virtual void clearBackground() = 0;

    virtual void drawTexture(int textureId, const Rect& source,
                             const Rect& dest, float rotation) = 0;
    virtual void drawRectangle(const Rect& rect,
                               int colorId) = 0;  // Enum or Id for colors
    virtual void drawText(const std::string& text, int posX, int posY,
                          int fontSize, int colorId) = 0;

    virtual int loadTexture(const std::string& filePath) = 0;
    virtual void unloadTexture(int textureId) = 0;
};

}  // namespace graphics
}  // namespace rteng