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

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & shown & width & height & outline.r & outline.g & outline.b & outline.a & inFill.r & inFill.g & inFill.b &
            inFill.a;
    }
};

}  // namespace comp
