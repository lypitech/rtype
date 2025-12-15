#pragma once

namespace comp {

struct MyColor
{
    unsigned char r;  // Color red value
    unsigned char g;  // Color green value
    unsigned char b;  // Color blue value
    unsigned char a;  // Color alpha value
};

struct Rectangle
{
    bool shown = true;
    float width = 0.0f;
    float height = 0.0;
    MyColor outline = {0, 0, 0, 0};
    MyColor inFill = {0, 0, 0, 0};

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & shown & width & height & outline & inFill;
    }
};

}  // namespace comp
