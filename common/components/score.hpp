#pragma once

namespace components {
struct Score
{
    int playerScore;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & playerScore;
    }
};
}  // namespace components
