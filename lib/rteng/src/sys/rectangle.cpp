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

        DrawRectangleLines(pos.x, pos.y, rect.width, rect.height, rect.outline);
        DrawRectangle(pos.x, pos.y, rect.width, rect.height, rect.inFill);
    }
}

}  // namespace sys
