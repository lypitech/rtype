#pragma once
#include <raylib.h>

namespace gui {

static constexpr std::string_view PLAYER_TEXTURE_FILEPATH =
    "../../Client/assets/sprites/Player.png";

class Texture
{
public:
    explicit Texture(const char* filepath,
                     const float scale = 1)
        : _texture(LoadTexture(filepath)),
          _scale(scale)
    {
    }

    ~Texture()
    {
        if (_texture.id != 0) {
            UnloadTexture(_texture);
        }
    }

    [[nodiscard]] const Texture2D& getTexture() const { return _texture; }
    [[nodiscard]] double getScale() const { return _scale; }

private:
    Texture2D _texture;
    double _scale;
};

}  // namespace gui
