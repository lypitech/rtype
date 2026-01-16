#pragma once

namespace components {

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
