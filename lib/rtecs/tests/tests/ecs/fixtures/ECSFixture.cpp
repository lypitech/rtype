#include "ECSFixture.hpp"

using namespace rtecs::tests::fixture;

void ECSFixture::SetUp()
{
    // Components
    _ecs.registerComponents<Profile>();
    _ecs.registerComponents<Hitbox>();
    _ecs.registerComponents<Health>();

    // ==================================
    //   Profile & Hitbox : From 0 to 2
    // ==================================
    _ecs.registerEntity<Profile, Hitbox>({.name = "L1x", .age = 20}, {.x = 0, .y = 0, .width = 20, .height = 20});
    _ecs.registerEntity<Profile, Hitbox>({.name = "Shuvly", .age = 19}, {.x = 10, .y = 10, .width = 20, .height = 20});
    _ecs.registerEntity<Profile, Hitbox>({.name = "Racaillou", .age = 20},
                                         {.x = 15, .y = 15, .width = 20, .height = 20});

    // ==================================
    //   Hitbox & Health : From 3 to 5
    // ==================================
    _ecs.registerEntity<Hitbox, Health>({.x = 0, .y = 0, .width = 20, .height = 20}, {.health = 20});
    _ecs.registerEntity<Hitbox, Health>({.x = 10, .y = 10, .width = 20, .height = 20}, {.health = 50});
    _ecs.registerEntity<Hitbox, Health>({.x = 15, .y = 15, .width = 20, .height = 20}, {.health = 100});

    // ==================================
    //   Health & Profile : From 6 to 8
    // ==================================
    _ecs.registerEntity<Health, Profile>({.health = 20}, {.name = "Babouye", .age = 20});
    _ecs.registerEntity<Health, Profile>({.health = 20}, {.name = "Electronicv", .age = 20});
    _ecs.registerEntity<Health, Profile>({.health = 20}, {.name = "I_dont_know", .age = 21});

    // Systems
    _ecs.registerSystem(std::make_unique<RenameOnLowLifeSystem>(_ecs));
    _ecs.registerSystem(std::make_unique<DamageOnCollideSystem>(_ecs));
    _ecs.registerSystem(std::make_unique<LogOnCollideSystem>(_ecs));
    _ecs.registerSystem(std::make_unique<MoveToCenterSystem>(_ecs));
}

void ECSFixture::TearDown()
{
    // Clear registered components, entities and systems
}

ECSFixture::RenameOnLowLifeSystem::RenameOnLowLifeSystem(const ECS& ecs)
    : ASystem(ecs.getMask<Health, Profile>())
{
}

void ECSFixture::RenameOnLowLifeSystem::apply(ECS& ecs)
{
    LOG_DEBUG("Going through rename on low life...");
    sparse::SparseGroup<Profile, Health> view = ecs.group<Profile, Health>();

    view.apply([&](Profile& profileComp, const Health& healthComp) {
        LOG_DEBUG("Applying to {}", profileComp.name);
        if (healthComp.health < 10) {
            profileComp.prefix = "[LOW]";
        } else {
            profileComp.prefix = "";
        }
    });
}

ECSFixture::DamageOnCollideSystem::DamageOnCollideSystem(const ECS& ecs)
    : ASystem(ecs.getMask<Health, Hitbox>())
{
}

void ECSFixture::DamageOnCollideSystem::apply(ECS& ecs)
{
    // Implement me...
}

ECSFixture::LogOnCollideSystem::LogOnCollideSystem(const ECS& ecs)
    : ASystem(ecs.getMask<Profile, Hitbox>())
{
}

void ECSFixture::LogOnCollideSystem::apply(ECS& ecs)
{
    // Implement me...
}

ECSFixture::MoveToCenterSystem::MoveToCenterSystem(const ECS& ecs)
    : ASystem(ecs.getMask<Hitbox>())
{
}

void ECSFixture::MoveToCenterSystem::apply(ECS& ecs)
{
    // Implement me...
}
