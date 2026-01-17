#include "button.hpp"

namespace gui {

Button::Button(const char* filepath,
               const Rectangle bounds,
               const float scale)
    : _bounds(bounds),
      _scale(scale)
{
    Image img = LoadImage(filepath);
    Image mask = GenImageColor(img.width, img.height, BLANK);

    int radius = std::min(img.width, img.height) * 0.17;

    ImageDrawRectangle(&mask, radius, 0, img.width - (radius * 2), img.height, WHITE);
    ImageDrawRectangle(&mask, 0, radius, img.width, img.height - (radius * 2), WHITE);
    ImageDrawCircle(&mask, radius, radius, radius, WHITE);
    ImageDrawCircle(&mask, img.width - radius, radius, radius, WHITE);
    ImageDrawCircle(&mask, radius, img.height - radius, radius, WHITE);
    ImageDrawCircle(&mask, img.width - radius, img.height - radius, radius, WHITE);

    ImageAlphaMask(&img, mask);
    UnloadImage(mask);
    _texture = LoadTextureFromImage(img);
    UnloadImage(img);
    if (_texture.id == 0) {
        LOG_ERR("Failed to load texture: {}", filepath);
    }
}

Button::~Button()
{
    if (_texture.id != 0 && IsWindowReady()) {
        UnloadTexture(_texture);
    }
}

bool Button::render() const
{
    const Vector2 mouse = GetMousePosition();

    const bool hover = CheckCollisionPointRec(mouse, _bounds);
    const bool pressed = hover && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    DrawTextureEx(_texture, {_bounds.x, _bounds.y}, 0, _scale, WHITE);
    return pressed;
}

}  // namespace gui
