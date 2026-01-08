#pragma once

#include <gtest/gtest.h>

namespace rtecs::tests::fixture {

/**
 * @brief This fixture offers 3 components :
 * -@code Profile@endcode: This component has a name attribute and an age attribute.
 * -@code Health@endcode: This component has a health attribute.
 * -@code Hitbox@endcode: This component has x and y attributes as coordinates, and with and height as a size.
 */
class ComponentFixture : public testing::Test
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
};

}  // namespace rtecs::tests::fixture
