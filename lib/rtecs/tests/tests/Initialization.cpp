#include <gtest/gtest.h>

#include "rtecs.h"
#include "logger/Logger.h"

TEST(a, a) {
    LOG_DEBUG("HI FROM TESTS");
    rtecs::hello();

    EXPECT_TRUE(true);
}

TEST(a, a2) {
    LOG_DEBUG("SECOND HI FROM TESTS LOL");
    LOG_FATAL(":(");

    EXPECT_TRUE(true);
}
