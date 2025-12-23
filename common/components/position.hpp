#pragma once

namespace components {

struct Position
{
    float x = 0.0f;
    float y = 0.0f;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & x & y;
    }
};

}  // namespace components
