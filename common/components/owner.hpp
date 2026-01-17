#pragma once

#include <cstdint>

#include "rtecs/types/types.hpp"

namespace components {

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
