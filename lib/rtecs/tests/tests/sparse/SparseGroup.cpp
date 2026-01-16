#include "rtecs/sparse/group/SparseGroup.hpp"

#include <gtest/gtest.h>

#include "fixtures/SparseGroupFixture.hpp"
#include "logger/Logger.h"
#include "rtecs/ECS.hpp"

using namespace rtecs::tests::fixture;
using namespace rtecs;

TEST_F(SparseGroupFixture,
       create_sparse_group)
{
    sparse::SparseGroup<Hitbox, Health> group(*_hitboxSet, *_healthSet);

    EXPECT_FALSE(group.has(0));
    EXPECT_TRUE(group.has(1));
    EXPECT_FALSE(group.has(2));

    const auto &hitboxView = group.getAllInstances<Hitbox>();
    const auto &healthView = group.getAllInstances<Health>();

    EXPECT_FALSE(hitboxView.has(0));
    EXPECT_TRUE(hitboxView.has(1));
    EXPECT_FALSE(hitboxView.has(2));

    EXPECT_FALSE(hitboxView.at(42).has_value());

    EXPECT_FALSE(healthView.has(0));
    EXPECT_TRUE(healthView.has(1));
    EXPECT_FALSE(healthView.has(2));

    EXPECT_FALSE(healthView.at(42).has_value());
};

TEST_F(SparseGroupFixture,
       edit_components_from_getEntity)
{
    sparse::SparseGroup<Hitbox, Health> group(*_hitboxSet, *_healthSet);

    const types::OptionalRef<Health> optionalComponent = group.getEntity<Health>(1);
    ASSERT_TRUE(optionalComponent.has_value());

    Health &component = optionalComponent.value();
    component.health += 42;

    const types::OptionalRef setComponent = _healthSet->get(1);
    ASSERT_TRUE(setComponent.has_value());
    EXPECT_EQ(setComponent.value().get().health, component.health);
};

TEST_F(SparseGroupFixture,
       edit_components_from_get)
{
    sparse::SparseGroup<Hitbox, Health> group(*_hitboxSet, *_healthSet);
    auto view = group.getAllInstances<Health>();

    ASSERT_TRUE(view.has(1));

    types::OptionalRef<Health> optionalComponent = view.at(1);
    ASSERT_TRUE(optionalComponent.has_value());

    Health &component = optionalComponent.value().get();
    component.health += 42;

    const types::OptionalRef setComponent = _healthSet->get(1);
    ASSERT_TRUE(setComponent.has_value());
    EXPECT_EQ(setComponent.value().get().health, component.health);
};

TEST_F(SparseGroupFixture,
       edit_components_from_getAll)
{
    sparse::SparseGroup<Hitbox, Health> group(*_hitboxSet, *_healthSet);
    auto &views = group.getAll();
    auto &view = std::get<sparse::SparseGroup<>::View<Health>>(views);

    ASSERT_TRUE(view.has(1));

    types::OptionalRef<Health> optionalHealthComp = view.at(1);

    ASSERT_TRUE(optionalHealthComp.has_value());

    Health &healthComp = optionalHealthComp.value().get();
    healthComp.health += 42;

    const types::OptionalRef setComponent = _healthSet->get(1);
    ASSERT_TRUE(setComponent.has_value());
    EXPECT_EQ(setComponent.value().get().health, healthComp.health);
};
