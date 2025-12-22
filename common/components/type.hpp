#pragma once

#include "enums/entity_types.hpp"

namespace comp {

struct Type
{
    entity::Type type;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & type;
    }
};

}  // namespace comp
