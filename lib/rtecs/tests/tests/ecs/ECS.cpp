#include "rtecs/ECS.hpp"

#include <gtest/gtest.h>

#include "fixtures/ECSFixture.hpp"
#include "logger/Logger.h"
#include "rtecs/sparse/group/SparseGroup.hpp"

using namespace rtecs::tests::fixture;
using namespace rtecs;

TEST_F(ECSFixture, ecs_initialisation)
{
    _ecs.applySystems();

    auto profileView = _ecs.group<Profile, Health>();

    profileView.apply([](const Profile &profileComp, const Health &healthComp) {
        if (healthComp.health < 10) {
            EXPECT_STREQ(profileComp.prefix.data(), "[LOW]");
        } else {
            EXPECT_STREQ(profileComp.prefix.data(), "");
        }
    });
};
