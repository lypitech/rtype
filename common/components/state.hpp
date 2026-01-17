#pragma once

namespace components {

/**
 * @brief Specifies the current state of an entity.
 */
struct State
{
    size_t state;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & state;
    }
};

}  // namespace components
