#pragma once

#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "rtecs/sparse/group/SparseGroup.hpp"

using namespace rtecs;

class SparseFixture : public testing::Test
{
protected:
    struct Profile
    {
        std::string name;
        char age;
    };

    struct Damageable
    {
        short health;
    };

    struct Collidable
    {
        int x;
        int y;
        int width;
        int height;
    };

    std::unique_ptr<sparse::SparseSet<Profile>> _profilesSet;
    std::unique_ptr<sparse::SparseSet<Damageable>> _damageablesSet;
    std::unique_ptr<sparse::SparseSet<Collidable>> _collidableSet;

    explicit SparseFixture();

    void TearDown() override
    {
        _profilesSet->clear();
        _damageablesSet->clear();
    }
};
