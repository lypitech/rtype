#include <string>
#include <unordered_map>

#include "Graphics.hpp"
#include "raylib.h"

namespace rteng {
namespace graphics {

class RaylibRenderer : public IRenderer
{
   private:
    Color MapColor(int colorId);

   public:
    std::unordered_map<int, Texture2D> m_textures;
    int m_nextTextureId = 1;

    void beginDrawing() override { BeginDrawing(); }

    void endDrawing() override { EndDrawing(); }

    void clearBackground() override;

    void drawTexture(int textureId, const Rect& source, const Rect& dest, float rotation) override;

    void drawRectangle(const Rect& rect, int colorId) override;

    void drawText(const std::string& text, int posX, int posY, int fontSize, int colorId) override;

    int loadTexture(const std::string& filePath) override;

    void unloadTexture(int textureId) override;
};

}  // namespace graphics
}  // namespace rteng
