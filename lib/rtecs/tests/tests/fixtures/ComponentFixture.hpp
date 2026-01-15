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
        std::string prefix = std::string("");
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

        /**
         * @brief Check for a collision between two Hitbox instances.
         *
         * @param other The other Hitbox component
         * @return `true` if a collision is detected, `false` otherwise.
         */
        [[nodiscard]]
        bool collideWith(const Hitbox &other) const;
    };
};

}  // namespace rtecs::tests::fixture
