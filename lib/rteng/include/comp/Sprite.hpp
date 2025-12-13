#pragma once

#include <raylib.h>

#include "position.hpp"

namespace comp {

struct Sprite
{
    bool shown = true;
    Texture2D texture = {};
    Position position = {0, 0};
    float scale = 1.0f;
    Color color = {0, 0, 0, 0};
};

}  // namespace comp
