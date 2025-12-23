#pragma once

#include "enums/entity_types.hpp"

namespace components {

struct Type
{
    entity::Type type;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & type;
    }
};

}  // namespace components
