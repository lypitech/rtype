#pragma once

#include <cstdint>

#include "rtecs/types/types.hpp"

namespace components {

struct Owner
{
    rtecs::types::EntityID id = 0;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id;
    }
};

}  // namespace components
