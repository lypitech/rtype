#include "Sprite.hpp"

#include "ECS.hpp"
#include "SparseSet.hpp"

namespace sys {

void Sprite::apply(rtecs::ECS&)
{
    // TODO: create.
}

// void hide(comp::Sprite& sprite) { sprite.shown = false; }
// void show(comp::Sprite& sprite) { sprite.shown = true; } // visibility system ?

void Animation::apply(rtecs::ECS&) { /* WIP */ }

}  // namespace sys
