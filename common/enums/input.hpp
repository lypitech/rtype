#pragma once

namespace game {

enum class Input
{
    kNone = 0,        // No input.
    kUp = 1 << 0,     // Go up.
    kDown = 1 << 1,   // Go down.
    kLeft = 1 << 2,   // Go to the left.
    kRight = 1 << 3,  // Go to the right.
    kShoot = 1 << 4   // Shoot the primary weapon.
};

}  // namespace game
