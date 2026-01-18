#pragma once

#include "enums/entity_types.hpp"

namespace components {

/**
 * @brief Specifies the type of the entity
 */
struct Type
{
    entity::Type type;

    auto operator<=>(const Type&) const = default;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & type;
    }
};

}  // namespace components
