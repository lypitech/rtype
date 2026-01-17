#pragma once

#include <cstdint>

namespace components {

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
