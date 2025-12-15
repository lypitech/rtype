#include "rectangle.hpp"

#include "ECS.hpp"
#include "SparseSet.hpp"
#include "rteng.hpp"

namespace sys {

void Rectangle::apply(rtecs::ECS& ecs)
{
    if (!rteng::GameEngine::getInstance().isClient()) {
        return;
    }
    for (auto [rect, pos] : ecs.view<comp::Rectangle, comp::Position>()) {
        if (!rect.shown) {
            continue;
        }

        Color col{rect.outline.r, rect.outline.g, rect.outline.b, rect.outline.a};
        DrawRectangleLines(pos.x, pos.y, rect.width, rect.height, col);
        col = Color{rect.inFill.r, rect.inFill.g, rect.inFill.b, rect.inFill.a};
        DrawRectangle(pos.x, pos.y, rect.width, rect.height, col);
    }
}

}  // namespace sys
