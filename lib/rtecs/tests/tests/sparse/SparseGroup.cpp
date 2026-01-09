#include "rtecs/sparse/group/SparseGroup.hpp"

#include <gtest/gtest.h>

#include "fixtures/SparseGroupFixture.hpp"
#include "logger/Logger.h"
#include "rtecs/ECS.hpp"

using namespace rtecs::tests::fixture;
using namespace rtecs;

TEST_F(SparseGroupFixture, create_sparse_group)
{
    sparse::SparseGroup group(*_hitboxSet, *_healthSet);

    EXPECT_FALSE(group.has(0));
    EXPECT_TRUE(group.has(1));
    EXPECT_FALSE(group.has(2));

    const auto &hitboxView = group.get<Hitbox>();
    const auto &healthView = group.get<Health>();

    EXPECT_FALSE(hitboxView.has(0));
    EXPECT_TRUE(hitboxView.has(1));
    EXPECT_FALSE(hitboxView.has(2));

    EXPECT_THROW(hitboxView[42], std::out_of_range);

    EXPECT_FALSE(healthView.has(0));
    EXPECT_TRUE(healthView.has(1));
    EXPECT_FALSE(healthView.has(2));

    EXPECT_THROW(healthView[42], std::out_of_range);
};

TEST_F(SparseGroupFixture, edit_components_from_getEntity)
{
    sparse::SparseGroup group(*_hitboxSet, *_healthSet);

    Health &component = group.getEntity<Health>(1).get();
    component.health += 42;

    const sparse::OptionalRef setComponent = _healthSet->get(1);
    ASSERT_TRUE(setComponent.has_value());
    EXPECT_EQ(setComponent.value().get().health, component.health);
};

TEST_F(SparseGroupFixture, edit_components_from_get)
{
    sparse::SparseGroup group(*_hitboxSet, *_healthSet);
    sparse::SparseGroup<Hitbox, Health>::View view = group.get<Health>();

    ASSERT_TRUE(view.has(1));

    Health &component = view[1];
    component.health += 42;

    const sparse::OptionalRef setComponent = _healthSet->get(1);
    ASSERT_TRUE(setComponent.has_value());
    EXPECT_EQ(setComponent.value().get().health, component.health);
};

TEST_F(SparseGroupFixture, edit_components_from_getAll)
{
    sparse::SparseGroup group(*_hitboxSet, *_healthSet);
    auto &views = group.getAll();
    auto &view = std::get<sparse::SparseGroup<>::View<Health>>(views);

    ASSERT_TRUE(view.has(1));

    Health &groupComponent = view[1];
    groupComponent.health += 42;

    const sparse::OptionalRef setComponent = _healthSet->get(1);
    ASSERT_TRUE(setComponent.has_value());
    EXPECT_EQ(setComponent.value().get().health, groupComponent.health);
};
