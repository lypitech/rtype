#pragma once

#include <raylib.h>

namespace comp {

struct Rectangle
{
    bool shown = true;
    float width = 0.0f;
    float height = 0.0;
    Color outline = {0, 0, 0, 0};
    Color inFill = {0, 0, 0, 0};
};

}  // namespace comp
