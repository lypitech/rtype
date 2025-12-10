#pragma once

#include <memory>
#include <string>
#include <vector>

namespace rteng::graphics {

struct Color
{
    unsigned char r;  // Color red value
    unsigned char g;  // Color green value
    unsigned char b;  // Color blue value
    unsigned char a;  // Color alpha value
};

constexpr Color LIGHTGRAY(200, 200, 200, 255);  // Light Gray
constexpr Color GRAY(130, 130, 130, 255);       // Gray
constexpr Color DARKGRAY(80, 80, 80, 255);      // Dark Gray
constexpr Color YELLOW(253, 249, 0, 255);       // Yellow
constexpr Color GOLD(255, 203, 0, 255);         // Gold
constexpr Color ORANGE(255, 161, 0, 255);       // Orange
constexpr Color PINK(255, 109, 194, 255);       // Pink
constexpr Color RED(230, 41, 55, 255);          // Red
constexpr Color MAROON(190, 33, 55, 255);       // Maroon
constexpr Color GREEN(0, 228, 48, 255);         // Green
constexpr Color LIME(0, 158, 47, 255);          // Lime
constexpr Color DARKGREEN(0, 117, 44, 255);     // Dark Green
constexpr Color SKYBLUE(102, 191, 255, 255);    // Sky Blue
constexpr Color BLUE(0, 121, 241, 255);         // Blue
constexpr Color DARKBLUE(0, 82, 172, 255);      // Dark Blue
constexpr Color PURPLE(200, 122, 255, 255);     // Purple
constexpr Color VIOLET(135, 60, 190, 255);      // Violet
constexpr Color DARKPURPLE(112, 31, 126, 255);  // Dark Purple
constexpr Color BEIGE(211, 176, 131, 255);      // Beige
constexpr Color BROWN(127, 106, 79, 255);       // Brown
constexpr Color DARKBROWN(76, 63, 47, 255);     // Dark Brown

constexpr Color WHITE(255, 255, 255, 255);     // White
constexpr Color BLACK(0, 0, 0, 255);           // Black
constexpr Color BLANK(0, 0, 0, 0);             // Blank (Transparent)
constexpr Color MAGENTA(255, 0, 255, 255);     // Magenta
constexpr Color RAYWHITE(245, 245, 245, 255);  // My own White (raylib logo)

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
    Renderer(int screenWidth, int screenHeight, const std::string& title, int fps = 60);

    ~Renderer();

    std::vector<std::shared_ptr<Texture2D>> _textures;
    int m_nextTextureId = 1;

    static void beginDrawing();

    static void endDrawing();

    static void clearBackground();

    static bool windowShouldClose();

    void drawTexture(int textureId, const Rect& source, const Rect& dest, float rotation);

    static void drawRectangle(const Rect& rect, const Color& color);

    static void drawText(const std::string& text, int posX, int posY, int fontSize, const Color& color);

    std::weak_ptr<Texture2D> loadTexture(const std::string& filePath);
};

}  // namespace rteng::graphics

