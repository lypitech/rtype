#pragma once

#include <cstdint>
#include "rtecs/rtecs.hpp"

namespace components {

struct Owner
{
    rtecs::EntityID id = 0;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id;
    }
};

}  // namespace components
