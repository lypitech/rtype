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
    auto movable = ecs.group<Velocity, Position, Hitbox>();
    auto colliders = ecs.group<Position, Hitbox>();

    movable.apply(
        [&](const rtecs::types::EntityID id, Velocity &vel, Position &pos, const Hitbox &box) {
            std::optional<std::pair<Position, Hitbox>> collider = std::nullopt;

            pos.isUpdated = true;
            colliders.apply([&](const rtecs::types::EntityID otherId,
                                const Position &otherPos,
                                const Hitbox &otherBox) {
                if (id == otherId || collider.has_value()) {
                    return;
                }

                const Position newRefPos = {pos.x + vel.vx, pos.y + vel.vy};
                bool isColliding = collide(newRefPos.x, box.width, otherPos.x, otherBox.width);
                isColliding &= collide(newRefPos.y, box.height, otherPos.y, otherBox.height);

                if (isColliding) {
                    collider = {otherPos, otherBox};
                }
            });
            applyHorizontalMovement(pos, vel, box, collider);
            applyVerticalMovement(pos, vel, box, collider);
        });
}

void ApplyMovement::applyHorizontalMovement(Position &pos,
                                            Velocity &vel,
                                            const Hitbox &box,
                                            const std::optional<std::pair<Position,
                                                                          Hitbox>> &collider)
{
    if (vel.vx != 0 && collider.has_value()) {
        if (vel.vx > 0) {  // Going right
            pos.x = collider.value().first.x - box.width;
        } else {  // Going left
            pos.x = collider.value().first.x + collider.value().second.width;
        }
    } else {
        pos.x += vel.vx;
    }
    vel.vx = 0;
}

void ApplyMovement::applyVerticalMovement(Position &pos,
                                          Velocity &vel,
                                          const Hitbox &box,
                                          const std::optional<std::pair<Position,
                                                                        Hitbox>> &collider)
{
    if (vel.vy != 0 && collider.has_value()) {
        if (vel.vy > 0) {  // Going down
            pos.y = collider.value().first.y - box.height;
        } else {  // Going up
            pos.y = collider.value().first.y + collider.value().second.height;
        }
    } else {
        pos.y += vel.vy;
    }
    vel.vy = 0;
}

bool ApplyMovement::collide(const float refPos,
                            const float refSize,
                            const float otherPos,
                            const float otherSize)
{
    return (refPos >= otherPos && refPos < (otherPos + otherSize)) ||
           (refPos + refSize > otherPos && refPos + refSize <= otherPos + otherSize);
}
