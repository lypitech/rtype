#pragma once
#include "utils.hpp"

namespace components {

struct Rectangle
{
    bool shown = true;
    float width = 0.0f;
    float height = 0.0;
    utils::MyColor outline = {0, 0, 0, 0};
    utils::MyColor inFill = {0, 0, 0, 0};

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & shown & width & height & outline & inFill;
    }
};

}  // namespace components
