#include <gtest/gtest.h>

#include "Tests.h"

int main(int argc,
         char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    testing::AddGlobalTestEnvironment(new LogEnvironment(argc, argv));
    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new LoggingListener);

    return RUN_ALL_TESTS();
}
