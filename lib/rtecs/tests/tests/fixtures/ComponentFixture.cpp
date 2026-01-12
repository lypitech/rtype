#include "ComponentFixture.hpp"

using namespace rtecs::tests::fixture;

bool ComponentFixture::Hitbox::collideWith(const Hitbox& other) const
{
    const bool horizontalCollision = x < (other.x + other.width) && x > other.x;
    const bool verticalCollision = y < (other.y + other.height) && y > other.y;

    return horizontalCollision && verticalCollision;
}
