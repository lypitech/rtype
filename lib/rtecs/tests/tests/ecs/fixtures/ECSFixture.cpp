#include "ECSFixture.hpp"

using namespace rtecs::tests::fixture;

void ECSFixture::SetUp()
{
    // Components
    _ecs.registerComponents<Profile, Hitbox, Health>();

    // Systems
    _ecs.registerSystem(std::make_shared<DamageOnCollideSystem>());
    _ecs.registerSystem(std::make_shared<MoveToCenterSystem>());
    _ecs.registerSystem(
        [](ECS& ecs) {
            sparse::SparseGroup<Profile, Health> view = ecs.group<Profile, Health>();

            view.apply([](types::EntityID, Profile& profileComp, const Health& healthComp) {
                if (healthComp.health < 10) {
                    profileComp.prefix = "[LOW]";
                } else {
                    profileComp.prefix = "";
                }
            });
        },
        "RenameOnLowLife");
}

void ECSFixture::TearDown()
{
    // Clear registered components, entities and systems
}

ECSFixture::DamageOnCollideSystem::DamageOnCollideSystem()
    : ASystem("DamageOnCollide")
{
}

void ECSFixture::DamageOnCollideSystem::apply(ECS& ecs)
{
    sparse::SparseGroup<Hitbox, Health> view = ecs.group<Hitbox, Health>();

    view.apply([&](types::EntityID entityId, Hitbox& hitbox, Health& health) {
        view.apply([&entityId, &hitbox, &health](
                       types::EntityID otherId, const Hitbox& otherHitbox, Health&) {
            if (hitbox.collideWith(otherHitbox)) {
                LOG_TRACE_R3(
                    "Collision detected between entity#{} and entity#{}.", entityId, otherId);
                health.health -= 5;
            }
        });
    });
}

ECSFixture::MoveToCenterSystem::MoveToCenterSystem()
    : ASystem("MoveToCenter")
{
}

void ECSFixture::MoveToCenterSystem::apply(ECS& ecs)
{
    sparse::SparseGroup<Hitbox> view = ecs.group<Hitbox>();

    view.apply([&](types::EntityID, Hitbox& hitboxComp) {
        if (hitboxComp.x > 0) {
            hitboxComp.x -= 5;
        }
        if (hitboxComp.x < 0) {
            hitboxComp.x += 5;
        }

        if (hitboxComp.y > 0) {
            hitboxComp.y -= 5;
        }
        if (hitboxComp.y < 0) {
            hitboxComp.y += 5;
        }
    });
}
