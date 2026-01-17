#pragma once

namespace components {

/**
 * @brief Stores the state of the collision of this entity
 */
struct Collision
{
    bool is_trigger = false;  // If true, we only detect collision but do not resolve it

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & is_trigger;
    }
};
}  // namespace components
