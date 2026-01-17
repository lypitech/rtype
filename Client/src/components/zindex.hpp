#pragma once

namespace components {
/**
 * @brief Specifies the layer to draw the entity
 */
struct ZIndex
{
    int layer;  // ex: 0 = background, 10 = players, 20 = UI overlay
};
}  // namespace components
