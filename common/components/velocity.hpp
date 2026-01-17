#pragma once

namespace components {

/**
 * @brief Specifies the max (potential) velocity and the current one (0 when not moving)
 */
struct Velocity
{
    float vx = 0.0f;
    float vy = 0.0f;
    float max_vx = 0.0f;
    float max_vy = 0.0f;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & vx & vy & max_vx & max_vy;
    }
};
}  // namespace components
