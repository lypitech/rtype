#pragma once

#include <cstdint>

namespace components {

struct MoveSet
{
    uint8_t set;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & set;
    }
};

}  // namespace components
