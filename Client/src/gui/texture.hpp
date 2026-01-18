#pragma once
#include <raylib.h>

#include <string_view>

#include "logger/Logger.h"

namespace gui {

#define ASSET_FILEPATH_PREFIX "Client/assets"
static constexpr std::string_view PLAYER_TEXTURE_FILEPATH = ASSET_FILEPATH_PREFIX "/sprites/player";
static constexpr std::string_view BACKGROUND_TEXTURE_FILEPATH =
    ASSET_FILEPATH_PREFIX "/background.png";

class Texture
{
public:
    explicit Texture(const char* filepath,
                     const float scale = 1)
        : _texture(LoadTexture(filepath)),
          _scale(scale)
    {
        if (_texture.id == 0) {
            LOG_ERR("Failed to load texture: {}", filepath);
        } else {
            LOG_INFO("Texture loaded successfully: {} (ID: {})", filepath, _texture.id);
        }
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
