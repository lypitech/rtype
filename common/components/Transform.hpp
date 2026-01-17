#pragma once

namespace components {

/**
 * @brief This component might later be an alternative to a position component
 */
struct Transform
{
    float x = 0;
    float y = 0;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & x & y;
    }
};

}  // namespace components
