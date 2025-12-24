#pragma once

namespace components {

struct Transform
{
    float x = 0;
    float y = 0;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & x & y;
    }
};

}  // namespace components
