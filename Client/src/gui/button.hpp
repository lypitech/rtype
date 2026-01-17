#pragma once
#include "texture.hpp"

namespace gui {

class Button
{
public:
    Button(const char* filepath,
           Rectangle bounds,
           float scale = 1);
    ~Button();

    [[nodiscard]] bool render() const;

private:
    Texture2D _texture;
    Rectangle _bounds;
    float _scale;
};

}  // namespace gui
