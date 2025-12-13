#pragma once

#include <raylib.h>

#include "position.hpp"

namespace comp {

struct Sprite
{
    bool _shown = true;
    Texture2D _texture{};
    Position position;
    float _scale = 1.0f;
    Color _color{};
    bool started = false;
};

}  // namespace comp
