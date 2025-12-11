#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "rtecs.h"

TEST(a, a)
{
    LOG_DEBUG("HI FROM TESTS");
    rtecs::hello();

    EXPECT_TRUE(true);
}

TEST(a, a2)
{
    LOG_DEBUG("SECOND HI FROM TESTS LOL");
    LOG_FATAL(":(");

    EXPECT_TRUE(true);
}
