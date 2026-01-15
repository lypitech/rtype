#include "IO.hpp"

#include <cstdint>

#include "enums/input.hpp"
#include "packets/client/user_input.hpp"
#include "raylib.h"
#include "rteng.hpp"

namespace systems {

/*
truth table
|previous | current |  result  |
|---------|---------|----------|
|   UP    |   UP    |    UP    |
|   UP    |  DOWN   | PRESSED  |
|  DOWN   |  DOWN   |   DOWN   |
|  DOWN   |   UP    | RELEASED |
*/
static button::State updateButtonState(const KeyboardKey key,
                                       button::State previousState)
{
    constexpr auto DOWN_BIT = static_cast<uint8_t>(button::State::DOWN);

    const bool isDown = IsKeyDown(key);
    const bool prevDown = (static_cast<uint8_t>(previousState) & DOWN_BIT) == DOWN_BIT;

    uint8_t out = 0;
    if (isDown != prevDown) {
        out = button::STATE_CHANGED_BIT;
    }
    out |= static_cast<uint8_t>(isDown);

    return static_cast<button::State>(out);
}

void IO::apply(rtecs::ECS&)
{
    _input.up = updateButtonState(KEY_UP, _input.up);
    _input.down = updateButtonState(KEY_DOWN, _input.down);
    _input.left = updateButtonState(KEY_LEFT, _input.left);
    _input.right = updateButtonState(KEY_RIGHT, _input.right);
    _input.action1 = updateButtonState(KEY_SPACE, _input.action1);
    _input.action2 = updateButtonState(KEY_LEFT_CONTROL, _input.action2);
    _input.mouse.x = GetMouseX();
    _input.mouse.y = GetMouseY();
    _input.mouse.leftButton = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    _input.mouse.rightButton = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
    packet::UserInput input{};
    if (_input.up == button::State::DOWN || _input.up == button::State::PRESSED) {
        input.input_mask |= static_cast<uint8_t>(game::Input::kUp);
    }
    if (_input.down == button::State::DOWN || _input.down == button::State::PRESSED) {
        input.input_mask |= static_cast<uint8_t>(game::Input::kDown);
    }
    if (_input.left == button::State::DOWN || _input.left == button::State::PRESSED) {
        input.input_mask |= static_cast<uint8_t>(game::Input::kLeft);
    }
    if (_input.right == button::State::DOWN || _input.right == button::State::PRESSED) {
        input.input_mask |= static_cast<uint8_t>(game::Input::kRight);
    }
    if (input.input_mask == 0) {
        return;
    }
    _service.send(input);
}

}  // namespace systems

