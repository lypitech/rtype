#pragma once

#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "rtecs/sparse/group/SparseGroup.hpp"

namespace rtecs::tests::fixture {

/**
 * This fixture offers:
 * - 3 components (Profile, Damageable, Collidable)
 * - 3 sets of components (_profileSet, _healthSet, _hitboxSet)
 */
class SparseFixture : public testing::Test
{
protected:
    struct Profile
    {
        std::string name;
        char age;
    };

    struct Health
    {
        short health;
    };

    struct Hitbox
    {
        int x;
        int y;
        int width;
        int height;
    };

    std::unique_ptr<sparse::SparseSet<Profile>> _profilesSet;
    std::unique_ptr<sparse::SparseSet<Health>> _healthSet;
    std::unique_ptr<sparse::SparseSet<Hitbox>> _hitboxSet;

    explicit SparseFixture();

    void TearDown() override
    {
        _profilesSet->clear();
        _healthSet->clear();
        _hitboxSet->clear();
    }
};

}  // namespace rtecs::tests::fixture
