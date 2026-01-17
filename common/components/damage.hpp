#pragma once

#include <cstdint>

namespace components {

/**
 * @brief Specify the damage the entity inflicts upon collision
 */
struct Damage
{
    uint32_t damage;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & damage;
    }
};

}  // namespace components
