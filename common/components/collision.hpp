#pragma once

namespace components {

/**
 * @brief Stores the state of the collision of this entity
 */
struct Collision
{
    bool isTriggered = false;  // If true, a collision has been detected.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & isTriggered;
    }
};
}  // namespace components
