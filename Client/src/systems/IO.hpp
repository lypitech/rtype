#pragma once

#include "components/position.hpp"
#include "network.hpp"
#include "rtecs/systems/ASystem.hpp"

namespace button {

static constexpr uint8_t STATE_CHANGED_BIT = 0b10;

enum class State : uint8_t
{
    UP = 0b00,        // is not pressed
    DOWN = 0b01,      // is being held down
    RELEASED = 0b10,  // just got released
    PRESSED = 0b11,   // just got pressed
};

}  // namespace button

struct IOValue
{
    struct Mouse
    {
        float x = 0;
        float y = 0;
        button::State leftButton;
        button::State rightButton;
    };
    button::State up;
    button::State down;
    button::State left;
    button::State right;
    button::State action1;
    button::State action2;
    button::State hitbox;
    Mouse mouse;
};

namespace systems {

class IO : public rtecs::systems::ASystem
{
public:
    explicit IO(service::Network& service)
        : ASystem("IO"),
          _service(service),
          _input()
    {
    }

    void apply(rtecs::ECS& ecs) override;

private:
    service::Network& _service;
    IOValue _input;
};

}  // namespace systems
