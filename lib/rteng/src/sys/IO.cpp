#include "IO.hpp"

#include <cstdint>

#include "ECS.hpp"
#include "SparseSet.hpp"
#include "comp/IO.hpp"
#include "raylib.h"

namespace sys {

// truth table
// previous | current |  result  |
// ---------|---------|----------|
//    UP    |   UP    |    UP    |
//    UP    |  DOWN   | PRESSED  |
//   DOWN   |  DOWN   |   DOWN   |
//   DOWN   |   UP    | RELEASED |
static comp::IO::ButtonState updateButtonState(KeyboardKey key, comp::IO::ButtonState previousState)
{
    constexpr uint8_t DOWN_BIT = static_cast<uint8_t>(comp::IO::ButtonState::DOWN);

    const bool isDown = IsKeyDown(key);
    const bool prevDown = (static_cast<uint8_t>(previousState) & DOWN_BIT) == DOWN_BIT;

    uint8_t out = 0;
    if (isDown != prevDown) {
        out = comp::IO::STATE_CHANGED_BIT;
    }
    out |= static_cast<uint8_t>(isDown);

    return static_cast<comp::IO::ButtonState>(out);
}

void IO::apply(rtecs::ECS& ecs)
{
    auto& ioComponents = ecs.getComponent<comp::IO>();
    rtecs::SparseSet<comp::IO>& ioSparseSet = dynamic_cast<rtecs::SparseSet<comp::IO>&>(ioComponents);

    for (auto& ioComp : ioSparseSet.getAll()) {
        ioComp.up = updateButtonState(KEY_UP, ioComp.up);
        ioComp.down = updateButtonState(KEY_DOWN, ioComp.down);
        ioComp.left = updateButtonState(KEY_LEFT, ioComp.left);
        ioComp.right = updateButtonState(KEY_RIGHT, ioComp.right);
        ioComp.action1 = updateButtonState(KEY_SPACE, ioComp.action1);
        ioComp.action2 = updateButtonState(KEY_LEFT_CONTROL, ioComp.action2);
        ioComp.mouse.x = GetMouseX();
        ioComp.mouse.y = GetMouseY();
        ioComp.mouse.leftButton = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        ioComp.mouse.rightButton = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
    }
}

}  // namespace sys

