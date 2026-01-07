#include "rtecs/sparse/group/SparseGroup.hpp"

#include <gtest/gtest.h>

#include "fixtures/SparseFixture.hpp"
#include "logger/Logger.h"
#include "rtecs/ECS.hpp"

using namespace rtecs;

TEST_F(SparseFixture, create_components_intersections)
{
    _collidableSet->put(0, {0, 0, 10, 10});
    _profilesSet->put(0, {"Carrot", 20});

    _collidableSet->put(1, {5, 5, 10, 10});
    _damageablesSet->put(1, {20});

    _profilesSet->put(2, {"Potato", 21});
    _damageablesSet->put(2, {15});

    sparse::SparseGroup collideAndDamage(_collidableSet, _damageablesSet);

    EXPECT_FALSE(collideAndDamage.has(0));
    EXPECT_TRUE(collideAndDamage.has(1));
    EXPECT_FALSE(collideAndDamage.has(2));

    const auto &cdCollideView = collideAndDamage.get<Collidable>();
    const auto &cdDamageView = collideAndDamage.get<Damageable>();

    EXPECT_FALSE(cdCollideView.has(0));
    EXPECT_TRUE(cdCollideView.has(1));
    EXPECT_FALSE(cdCollideView.has(2));

    EXPECT_FALSE(cdDamageView.has(0));
    EXPECT_TRUE(cdDamageView.has(1));
    EXPECT_FALSE(cdDamageView.has(2));

    sparse::SparseGroup profileAndCollide(_profilesSet, _collidableSet);

    EXPECT_TRUE(profileAndCollide.has(0));
    EXPECT_FALSE(profileAndCollide.has(1));
    EXPECT_FALSE(profileAndCollide.has(2));

    const auto &pcProfileView = profileAndCollide.get<Profile>();
    const auto &pcCollideView = profileAndCollide.get<Collidable>();

    EXPECT_TRUE(pcProfileView.has(0));
    EXPECT_FALSE(pcProfileView.has(1));
    EXPECT_FALSE(pcProfileView.has(2));

    EXPECT_TRUE(pcCollideView.has(0));
    EXPECT_FALSE(pcCollideView.has(1));
    EXPECT_FALSE(pcCollideView.has(2));
};
