#include "apply_movement.hpp"

#include "components/collision.hpp"
#include "components/factory.hpp"
#include "components/position.hpp"
#include "components/velocity.hpp"
#include "rtecs/ECS.hpp"

using namespace server::systems;
using namespace components;

ApplyMovement::ApplyMovement()
    : ASystem("UpdatePosition")
{
}

void ApplyMovement::apply(rtecs::ECS &ecs)
{
    using namespace components;
    auto group = ecs.group<Velocity, Position, Collision>();

    group.apply([&](const rtecs::types::EntityID,
                    const Velocity &vel,
                    Position &pos,
                    const Collision &box) {
        group.apply([&](const rtecs::types::EntityID,
                        Velocity &,
                        const Position &otherPos,
                        const Collision &otherBox) {
            const Position newRefPos = {pos.x + vel.vx, pos.y + vel.vy};

            if (!checkCollision(newRefPos.x, box.width, otherPos.x, otherBox.width)) {
                pos.x += vel.vx;
                pos.isUpdated = true;
            }
            if (checkCollision(newRefPos.y, box.height, otherPos.y, otherBox.height)) {
                pos.y += vel.vy;
                pos.isUpdated = true;
            }
        });
    });
}

bool ApplyMovement::checkCollision(const float refPos,
                                   const float refSize,
                                   const float otherPos,
                                   const float otherSize)
{
    return (refPos > otherPos && refPos < (otherPos + otherSize)) ||
           (refPos + refSize > otherPos && refPos + refSize < otherPos + otherSize);
}
