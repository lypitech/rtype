#include "Sprite.hpp"

#include "ECS.hpp"
#include "SparseSet.hpp"

namespace sys {

void Sprite::apply(rtecs::ECS& ecs)
{
    rtecs::ISparseSet& spriteComponents = ecs.getComponent<comp::Sprite>();
    auto& spritesSparseSet = dynamic_cast<rtecs::SparseSet<comp::Sprite>&>(spriteComponents);

    for (const auto& [shown, texture, position, scale, color] : spritesSparseSet.getAll()) {
        if (!shown) {
            return;
        }
        const Vector2 pos = {position.x, position.y};
        DrawTextureEx(texture, pos, 0.0f, scale, color);
    }
}

// void hide(comp::Sprite& sprite) { sprite.shown = false; }
// void show(comp::Sprite& sprite) { sprite.shown = true; } // visibility system ?

void Animation::apply(rtecs::ECS&) { /* WIP */ }

}  // namespace sys
