#pragma once

namespace components {

struct Hitbox
{
    bool shown = false;
    float width = 0.0f;
    float height = 0.0f;
    float x = 0.0f;
    float y = 0.0f;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & shown;
        ar & width;
        ar & height;
        ar & x;
        ar & y;
    }
};

}  // namespace components
