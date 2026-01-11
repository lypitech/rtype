#pragma once
#include "enums/entity_types.hpp"

namespace components {

struct Sprite
{
    entity::Type type;

    // Possible future extensions:
    // float scale = 1.0f;
    // std::string texture_path;   // Alternatively, we can use an ID to reference a texture in an asset manager
                                // (btw that's better for performance and idk if it was intended with the enum above)
};

}  // namespace components
