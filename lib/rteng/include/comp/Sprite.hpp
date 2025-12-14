#pragma once

#include <raylib.h>

#include "position.hpp"

namespace comp {

struct Sprite
{
    bool shown = true;
    Texture2D texture = {};
    float scale = 1.0f;
    Color color = {0, 0, 0, 0};

    template <typename Archive>
    void serialize(Archive&)
    {
        // LALALALALLA J4ENTENDS PAS
    }
};

}  // namespace comp
