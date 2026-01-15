#pragma once
#include <cstdint>

namespace components {

struct Damage
{
    int32_t value = 0;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & value;
    }
};
}  // namespace components
