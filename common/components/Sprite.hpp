#pragma once

#include "rect.hpp"

namespace comp {

struct Sprite
{
    bool shown = true;
    float scale = 1.0f;
    MyColor color = {0, 0, 0, 0};

    template <typename Archive>
    void serialize(Archive&)
    {
        // LALALALALLA J4ENTENDS PAS
    }
};

}  // namespace comp
