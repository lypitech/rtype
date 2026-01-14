#include "ECSFixture.hpp"

using namespace rtecs::tests::fixture;

void ECSFixture::SetUp()
{
    // Components
    _ecs.registerComponents<Profile, Hitbox, Health>();
    // _ecs.registerComponents<Hitbox>();
    // _ecs.registerComponents<Health>();

    // ==================================
    //   Profile & Hitbox : From 0 to 2
    // ==================================
    const types::EntityID firstEntityId = _ecs.registerEntity<Profile, Hitbox>(
        {.name = "L1x", .age = 20}, {.x = 0, .y = 0, .width = 20, .height = 20});
    const types::EntityID secondEntityId = _ecs.registerEntity<Profile, Hitbox>(
        {.name = "Shuvly", .age = 19}, {.x = 10, .y = 10, .width = 20, .height = 20});
    const types::EntityID thirdEntityId = _ecs.registerEntity<Profile, Hitbox>(
        {.name = "Racaillou", .age = 20}, {.x = 15, .y = 15, .width = 20, .height = 20});

    LOG_INFO("Registering entity {} with components <Profile, Hitbox>.", firstEntityId);
    LOG_INFO("Registering entity {} with components <Profile, Hitbox>.", secondEntityId);
    LOG_INFO("Registering entity {} with components <Profile, Hitbox>.", thirdEntityId);

    // ==================================
    //   Hitbox & Health : From 3 to 5
    // ==================================
    _ecs.registerEntity<Hitbox, Health>(
        {.x = 0, .y = 0, .width = 20, .height = 20}, {.health = 20});
    _ecs.registerEntity<Hitbox, Health>(
        {.x = 10, .y = 10, .width = 20, .height = 20}, {.health = 50});
    _ecs.registerEntity<Hitbox, Health>(
        {.x = 15, .y = 15, .width = 20, .height = 20}, {.health = 100});

    // ==================================
    //   Health & Profile : From 6 to 8
    // ==================================
    _ecs.registerEntity<Health, Profile>({.health = 20}, {.name = "Babouye", .age = 20});
    _ecs.registerEntity<Health, Profile>({.health = 20}, {.name = "Electronicv", .age = 20});
    _ecs.registerEntity<Health, Profile>({.health = 20}, {.name = "I_dont_know", .age = 21});

    // Systems
    _ecs.registerSystem(std::make_unique<DamageOnCollideSystem>());
    _ecs.registerSystem(std::make_unique<MoveToCenterSystem>());
    _ecs.registerSystem(
        [&](ECS& ecs) {
            sparse::SparseGroup<Profile, Health> view = ecs.group<Profile, Health>();

            view.apply([&](types::EntityID entityId, Profile& profileComp, Health& healthComp) {
                if (healthComp.health < 10) {
                    LOG_TRACE_R3(
                        "Entity n°{} (Profile: {}) is low life !", entityId, profileComp.name);
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

    // TODO : Remove throws (replace by LOG_WARN)
    // TODO : Position &ecs.getEntity<Position>(id);
    // TODO : std::tuple<Position&, Health&> &ecs.getEntity<Position>(id);
    // TODO : ecs.destroyEntity(id);
    // TODO : ecs.updateEntity<Position, Health>(id, { .x = 10, .y = 10 }, { .hp = 10 });
    // ecs.addEntityComponent<Hitbox, Health>(10, { 10, 10, 10, 10 }, {10})

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
