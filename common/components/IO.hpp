#pragma once

#include <cstdint>

namespace components {

struct IO
{
    static constexpr uint8_t STATE_CHANGED_BIT = 0b10;

    enum class ButtonState : uint8_t
    {
        UP = 0b00,        // is not pressed
        DOWN = 0b01,      // is being held down
        RELEASED = 0b10,  // just got released
        PRESSED = 0b11,   // just got pressed
    };
    struct Mouse
    {
        float x = 0;
        float y = 0;
        bool leftButton = false;
        bool rightButton = false;

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar & x & y & leftButton & rightButton;
        }
    };
    ButtonState up;
    ButtonState down;
    ButtonState left;
    ButtonState right;
    ButtonState action1;
    ButtonState action2;
    Mouse mouse;
    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & up & down & left & right & action1 & action2 & mouse;
    }
};

}  // namespace components
