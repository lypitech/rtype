#pragma once

#include <cstdint>

namespace components {

/**
 * @brief Specifies the value of the entity (used for score upon kill)
 */
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
