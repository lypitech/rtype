#include "rtecs/ECS.hpp"

#include <gtest/gtest.h>

#include "fixtures/ECSFixture.hpp"
#include "logger/Logger.h"
#include "rtecs/sparse/group/SparseGroup.hpp"

using namespace rtecs::tests::fixture;
using namespace rtecs;

TEST_F(ComponentFixture,
       register_components_in_one_line)
{
    ECS ecs;

    ecs.registerComponents<Profile, Health, Hitbox>();

    const bitset::DynamicBitSet mask = ecs.getComponentMask<Profile, Health, Hitbox>();
    const bitset::DynamicBitSet expectedMask(
        {std::bitset<64>{0b0111000000000000000000000000000000000000000000000000000000000000}});
    EXPECT_EQ(mask, expectedMask);

    const bitset::DynamicBitSet healthMask = ecs.getComponentMask<Health>();
    const bitset::DynamicBitSet expectedHealthMask(
        {std::bitset<64>{0b0010000000000000000000000000000000000000000000000000000000000000}});
    EXPECT_EQ(healthMask, expectedHealthMask);
}

TEST_F(ComponentFixture,
       register_components_in_multiple_line)
{
    ECS ecs;

    ecs.registerComponents<Profile, Health>();
    ecs.registerComponents<Hitbox>();

    const bitset::DynamicBitSet mask = ecs.getComponentMask<Profile, Health, Hitbox>();
    const bitset::DynamicBitSet expectedMask(
        {std::bitset<64>{0b0111000000000000000000000000000000000000000000000000000000000000}});
    EXPECT_EQ(mask, expectedMask);

    const bitset::DynamicBitSet healthMask = ecs.getComponentMask<Health>();
    const bitset::DynamicBitSet expectedHealthMask(
        {std::bitset<64>{0b0010000000000000000000000000000000000000000000000000000000000000}});
    EXPECT_EQ(healthMask, expectedHealthMask);
}

TEST_F(ComponentFixture,
       register_entity_with_multiple_components)
{
    ECS ecs;

    ecs.registerComponents<Profile, Health>();
    ecs.registerComponents<Hitbox>();

    const types::EntityID id = ecs.registerEntity<Profile, Health>(
        {.prefix = "", .name = "L1x", .age = 20}, {.health = 20});
    EXPECT_EQ(id, 0);

    const bitset::DynamicBitSet &mask = ecs.getEntityMask(id);
    const bitset::DynamicBitSet expectedMask(
        {std::bitset<64>{0b0110000000000000000000000000000000000000000000000000000000000000}});
    EXPECT_EQ(mask, expectedMask);
}

TEST_F(ComponentFixture,
       register_entity_without_any_components)
{
    ECS ecs;

    const types::EntityID id = ecs.preRegisterEntity();
    EXPECT_EQ(id, 0);

    const bitset::DynamicBitSet &mask = ecs.getEntityMask(id);
    const bitset::DynamicBitSet expectedMask{};
    EXPECT_EQ(mask, expectedMask);
}

TEST_F(ECSFixture,
       update_entity)
{
    types::EntityID entityId = _ecs.registerEntity<Profile, Health>({"", "L1x", 20}, {20});

    ASSERT_TRUE(entityId != types::NullEntityID);
    EXPECT_TRUE(_ecs.updateEntity<Profile>(entityId, {"", "L2x", 21}));

    sparse::SparseGroup<Profile> group = _ecs.group<Profile>();

    ASSERT_TRUE(group.has(entityId));
    group.apply([&entityId](const types::EntityID id, const Profile &profileComp) {
        if (id == entityId) {
            EXPECT_STREQ(profileComp.name.data(), "L2x");
            EXPECT_EQ(profileComp.age, 21);
        }
    });
}

TEST_F(ECSFixture,
       update_invalid_entity)
{
    EXPECT_FALSE(_ecs.updateEntity<Profile>(42, {"", "L2x", 21}));
}

TEST_F(ECSFixture,
       add_entity_component)
{
    const types::EntityID entityId = _ecs.registerEntity<Profile, Health>(
        {.prefix = "", .name = "L1x", .age = 20}, {.health = 20});
    bitset::DynamicBitSet entityMask = _ecs.getEntityMask(entityId);
    bitset::DynamicBitSet expectedEntityMask =
        _ecs.getComponentMask<Profile>() | _ecs.getComponentMask<Health>();
    ASSERT_EQ(entityMask, expectedEntityMask);

    _ecs.addEntityComponents<Hitbox>(entityId, {0, 0, 10, 10});

    entityMask = _ecs.getEntityMask(entityId);
    expectedEntityMask |= _ecs.getComponentMask<Hitbox>();
    ASSERT_EQ(entityMask, expectedEntityMask);

    sparse::SparseGroup<Hitbox> group = _ecs.group<Hitbox>();
    ASSERT_TRUE(group.has(entityId));

    const types::OptionalCRef<Hitbox> optHitboxComp = group.getEntity<Hitbox>(entityId);
    ASSERT_TRUE(optHitboxComp.has_value());

    const Hitbox &hitboxComp = optHitboxComp.value().get();
    EXPECT_EQ(hitboxComp.x, 0);
}

TEST_F(ECSFixture,
       destroy_entity)
{
    const types::EntityID entityId = _ecs.registerEntity<Profile, Health>({"", "L1x", 20}, {20});

    _ecs.destroyEntity(entityId);

    sparse::SparseGroup<Profile, Health> group = _ecs.group<Profile, Health>();

    const types::EntityID newEntityId = _ecs.registerEntity<Profile, Health>({"", "L1x", 20}, {20});
    sparse::SparseGroup<Profile, Health> newGroup = _ecs.group<Profile, Health>();
    EXPECT_TRUE(entityId != newEntityId);
    EXPECT_FALSE(group.has(entityId));
    EXPECT_FALSE(newGroup.has(entityId));
    EXPECT_TRUE(newGroup.has(newEntityId));

    bitset::DynamicBitSet entityMask = _ecs.getEntityMask(entityId);
    bitset::DynamicBitSet expectedMask{};
    ASSERT_EQ(entityMask, expectedMask);
}

TEST_F(ECSFixture,
       apply_systems)
{
    auto profileView = _ecs.group<Profile, Health>();
    _ecs.applyAllSystems();

    profileView.apply(
        [](const types::EntityID &, const Profile &profileComp, const Health &healthComp) {
            if (healthComp.health < 10) {
                EXPECT_STREQ(profileComp.prefix.data(), "[LOW]");
            } else {
                EXPECT_STREQ(profileComp.prefix.data(), "");
            }
        });
};
