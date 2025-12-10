#include <gtest/gtest.h>

#include "../../include/rteng.hpp"

TEST(Initialisation, EngineConstructor)
{
    rteng::GameEngine game(800, 600, "rteng Test Runner");

    EXPECT_TRUE(true);
}
