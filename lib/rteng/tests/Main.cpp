#include <gtest/gtest.h>

#include "../include/rteng.hpp"
#include "TestEnvironment.hpp"

// Add a flag or argument to control execution type
int main(int argc, char* argv[])
{
    bool is_automated_test = false;  // Implement a check here if possible

    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        is_automated_test = true;
    }

    if (is_automated_test) {
        testing::AddGlobalTestEnvironment(new RaylibTestEnvironment);
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    // --- Interactive Mode ---
    rteng::GameEngine engine(800, 600, "R-Type Client Interactive");
    engine.run();

    return 0;
}
