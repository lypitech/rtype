#pragma once

#include <cstdint>

namespace comp {

class IO
{
public:
    static constexpr uint8_t STATE_CHANGED_BIT = 0b10;

    enum class ButtonState : uint8_t
    {
        UP = 0b00,        // is not pressed
        DOWN = 0b01,      // is being held down
        RELEASED = 0b10,  // just got released
        PRESSED = 0b11,   // just got pressed
    };
    class Mouse
    {
    public:
        float x = 0;
        float y = 0;
        bool leftButton = false;
        bool rightButton = false;
    };
    ButtonState up;
    ButtonState down;
    ButtonState left;
    ButtonState right;
    ButtonState action1;
    ButtonState action2;
    Mouse mouse;
};

}  // namespace comp
