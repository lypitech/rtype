#include "apply_movement.hpp"

#include "components/factory.hpp"
#include "components/position.hpp"
#include "components/velocity.hpp"
#include "enums/player_state.hpp"
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
    auto movable = ecs.group<Type, Velocity, Position, Hitbox, State>();
    auto colliders = ecs.group<Position, Hitbox, State, Type>();

    movable.apply([&](const rtecs::types::EntityID id,
                      const Type& type,
                      Velocity& vel,
                      Position& pos,
                      const Hitbox& box,
                      State& state) {
        pos.isUpdated = vel.vx != 0 || vel.vy != 0;
        if (type.type == entity::Type::kPlayer) {
            const Position nextHorizontalPos = {pos.x + vel.vx, pos.y};
            const std::optional<Collider> horizontalCollider =
                findCollider(id, nextHorizontalPos, box, colliders, entity::Type::kPlayer);
            handlePlayerHorizontalMovement(pos, vel, box, horizontalCollider);

            const Position nextVerticalPos = {pos.x, pos.y + vel.vy};
            const std::optional<Collider> verticalCollider =
                findCollider(id, nextVerticalPos, box, colliders, entity::Type::kPlayer);
            handlePlayerVerticalMovement(pos, vel, box, verticalCollider);
        } else {
            const Position nextPos = {pos.x + vel.vx, pos.y + vel.vy};
            const entity::Type expectedType =
                type.type == entity::Type::kBullet ? entity::Type::kEnemy : entity::Type::kPlayer;
            const std::optional<Collider> collider =
                findCollider(id, nextPos, box, colliders, expectedType);
            handleEntityMovement(pos, vel, box, state, collider);
            if (type.type == entity::Type::kBullet &&
                (pos.x - box.width < 0 || pos.x > 1920 || pos.y - box.height < 0 || pos.y > 1080)) {
                state.state = entity::state::EntityDead;
            }
        }
    });
}

std::optional<ApplyMovement::Collider> ApplyMovement::findCollider(
    const rtecs::types::EntityID id,
    const Position& pos,
    const Hitbox& box,
    rtecs::sparse::SparseGroup<Position,
                               Hitbox,
                               State,
                               Type>& colliders,
    const entity::Type expectedColliderType)
{
    bool isCollisionDetected = false;
    std::optional<Collider> collider = std::nullopt;

    colliders.apply([&](const rtecs::types::EntityID colliderId,
                        Position& colliderPos,
                        Hitbox& colliderBox,
                        State& colliderState,
                        Type& colliderType) {
        if (colliderType.type != expectedColliderType || colliderId == id || isCollisionDetected) {
            return;
        }
        if (collide(pos, box, colliderPos, colliderBox)) {
            isCollisionDetected = true;
            collider = std::tuple<Position&, Hitbox&, State&, Type&>(
                colliderPos, colliderBox, colliderState, colliderType);
        }
    });
    return collider;
}

void ApplyMovement::handleEntityMovement(Position& pos,
                                         Velocity& vel,
                                         const Hitbox&,
                                         State& state,
                                         const std::optional<Collider>& collider)
{
    if (collider.has_value()) {
        // Stop the entity movement, probably useless, but we never know
        vel.vx = 0;
        vel.vy = 0;
        // Kill the movable entity and the collided entity
        std::get<State&>(collider.value()).state = entity::state::EntityDead;
        state.state = entity::state::EntityDead;
    } else {
        pos.x += vel.vx;
        pos.y += vel.vy;
    }
}

void ApplyMovement::handlePlayerHorizontalMovement(Position& pos,
                                                   Velocity& vel,
                                                   const Hitbox& box,
                                                   const std::optional<Collider>& collider)
{
    if (collider.has_value()) {
        const Position& colliderPos = std::get<Position&>(collider.value());
        const Hitbox& colliderBox = std::get<Hitbox&>(collider.value());
        if (vel.vx > 0) {  // Right
            pos.x = colliderPos.x - box.width;
        } else if (vel.vx < 0) {  // Left
            pos.x = colliderPos.x + colliderBox.width;
        }
    } else {
        pos.x += vel.vx;
    }
    vel.vx = 0;
}

void ApplyMovement::handlePlayerVerticalMovement(Position& pos,
                                                 Velocity& vel,
                                                 const Hitbox& box,
                                                 const std::optional<Collider>& collider)
{
    if (collider.has_value()) {
        const Position& colliderPos = std::get<Position&>(collider.value());
        const Hitbox& colliderBox = std::get<Hitbox&>(collider.value());
        if (vel.vy > 0) {  // Down
            pos.y = colliderPos.y - box.height;
        } else if (vel.vy < 0) {  // Up
            pos.y = colliderPos.y + colliderBox.height;
        }
    } else {
        pos.y += vel.vy;
    }
    vel.vy = 0;
}

bool ApplyMovement::collide(const Position& refPos,
                            const Hitbox& refBox,
                            const Position& otherPos,
                            const Hitbox& otherBox)
{
    return (refPos.x < otherPos.x + otherBox.width && refPos.x + refBox.width > otherPos.x &&
            refPos.y < otherPos.y + otherBox.height && refPos.y + refBox.height > otherPos.y);
}
