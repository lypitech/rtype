#pragma once

#include <cstdint>

namespace components {

struct Value
{
    uint32_t value;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & value;
    }
};

}  // namespace components
