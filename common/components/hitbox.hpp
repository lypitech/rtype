#pragma once

namespace components {

/**
 * @brief Contains a bool whether it has to be shown to user along with the size of the hitbox.
 */
struct Hitbox
{
    bool shown = false;
    float width = 0.0f;
    float height = 0.0f;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & shown;
        ar & width;
        ar & height;
    }
};

}  // namespace components
