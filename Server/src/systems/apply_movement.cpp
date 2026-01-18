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

void ApplyMovement::apply(rtecs::ECS& ecs)
{
    using namespace components;
    auto movable = ecs.group<Velocity, Position, Hitbox>();
    auto colliders = ecs.group<Position, Hitbox>();

    movable.apply(
        [&](const rtecs::types::EntityID id, Velocity& vel, Position& pos, const Hitbox& box) {
            handleHorizontalMovement(id, pos, vel, box, colliders);
            handleVerticalMovement(id, pos, vel, box, colliders);
            vel.vx = 0;
            vel.vy = 0;
        });
}

void ApplyMovement::handleHorizontalMovement(const rtecs::types::EntityID id,
                                             Position& pos,
                                             const Velocity& vel,
                                             const Hitbox& box,
                                             rtecs::sparse::SparseGroup<Position,
                                                                        Hitbox>& colliders)
{
    const Position nextPos = {pos.x + vel.vx, pos.y};
    bool isCollisionDetected = false;

    colliders.apply([&](const rtecs::types::EntityID colliderId,
                        const Position& colliderPos,
                        const Hitbox& colliderBox) {
        if (colliderId == id || isCollisionDetected) {
            return;
        }

        if (collide(nextPos, box, colliderPos, colliderBox)) {
            isCollisionDetected = true;
            if (vel.vx > 0) {  // Right
                pos.isUpdated = true;
                pos.x = colliderPos.x - box.width;
            } else if (vel.vx < 0) {  // Left
                pos.isUpdated = true;
                pos.x = colliderPos.x + colliderBox.width;
            }
        }
    });

    if (!isCollisionDetected) {
        pos.isUpdated = true;
        pos.x = nextPos.x;
    }
}

void ApplyMovement::handleVerticalMovement(const rtecs::types::EntityID id,
                                           Position& pos,
                                           const Velocity& vel,
                                           const Hitbox& box,
                                           rtecs::sparse::SparseGroup<Position,
                                                                      Hitbox>& colliders)
{
    const Position nextPos = {pos.x, pos.y + vel.vy};
    bool isCollisionDetected = false;

    colliders.apply([&](const rtecs::types::EntityID colliderId,
                        const Position& colliderPos,
                        const Hitbox& colliderBox) {
        if (colliderId == id || isCollisionDetected) {
            return;
        }

        if (collide(nextPos, box, colliderPos, colliderBox)) {
            isCollisionDetected = true;
            if (vel.vy > 0) {  // Down
                pos.isUpdated = true;
                pos.y = colliderPos.y - box.height;
            } else if (vel.vy < 0) {  // Up
                pos.isUpdated = true;
                pos.y = colliderPos.y + colliderBox.height;
            }
        }
    });

    if (!isCollisionDetected) {
        pos.isUpdated = true;
        pos.y = nextPos.y;
    }
}

bool ApplyMovement::collide(const Position& refPos,
                            const Hitbox& refBox,
                            const Position& otherPos,
                            const Hitbox& otherBox)
{
    return (refPos.x < otherPos.x + otherBox.width && refPos.x + refBox.width > otherPos.x &&
            refPos.y < otherPos.y + otherBox.height && refPos.y + refBox.height > otherPos.y);
}
