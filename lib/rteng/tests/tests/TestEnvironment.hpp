#include <gtest/gtest.h>
#include <raylib.h>

#include "../../include/Renderer.h"

class RaylibTestEnvironment : public testing::Environment
{
   public:
    // Before each test
    void SetUp()
    {
        InitWindow(1, 1, "GTest Raylib Context");
        SetTargetFPS(60);
    }

    // After each test
    void TearDown() { CloseWindow(); }
};
