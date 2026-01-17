#pragma once

#include <cstdint>

#include "rtecs/types/types.hpp"

namespace components {

/**
 * @brief Specifies the id of the owner of this entity (used for bullets)
 */
struct Owner
{
    rtecs::types::EntityID id;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id;
    }
};

}  // namespace components
