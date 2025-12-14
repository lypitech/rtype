#include "rectangle.hpp"

#include "ECS.hpp"
#include "SparseSet.hpp"

namespace sys {

void Rectangle::apply(rtecs::ECS& ecs)
{
    for (auto [rect, pos] : ecs.view<comp::Rectangle, comp::Position>()) {
        if (!rect.shown) {
            continue;
        }
        LOG_INFO("Drawing rectangle of ({},{}) at ({}, {})", rect.width, rect.height, pos.x, pos.y);

        DrawRectangle(pos.x, pos.y, rect.width, rect.height, rect.inFill);
    }
}

}  // namespace sys
