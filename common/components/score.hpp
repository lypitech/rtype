#pragma once

namespace components {
/**
 * @brief Specify the current score of a player
 */
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
