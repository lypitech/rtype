#pragma once

#include <cstdint>

namespace components {

/**
 * @brief Specify the max and current health of the entity.
 */
struct Health
{
    uint32_t hp = 100;
    uint32_t max_hp = 100;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & hp & max_hp;
    }
};

}  // namespace components
