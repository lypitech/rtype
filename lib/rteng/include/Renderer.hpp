#include <string>
#include <unordered_map>

namespace rteng::graphics {

struct Color
{
    unsigned char r;  // Color red value
    unsigned char g;  // Color green value
    unsigned char b;  // Color blue value
    unsigned char a;  // Color alpha value
};

struct Rect
{
    float x;
    float y;
    float width;
    float height;
};

struct Texture2D
{
    unsigned int id;  // OpenGL texture id
    int width;        // Texture base width
    int height;       // Texture base height
    int mipmaps;      // Mipmap levels, 1 by default
    int format;       // Data format (PixelFormat type)
};

class Renderer
{
   public:
    std::unordered_map<int, Texture2D> m_textures;
    int m_nextTextureId = 1;

    static void beginDrawing();

    static void endDrawing();

    static void clearBackground();

    void drawTexture(int textureId, const Rect& source, const Rect& dest, float rotation);

    static void drawRectangle(const Rect& rect, const Color& color);

    static void drawText(const std::string& text, int posX, int posY, int fontSize, const Color& color);

    int loadTexture(const std::string& filePath);

    void unloadTexture(int textureId);
};

}  // namespace rteng::graphics

