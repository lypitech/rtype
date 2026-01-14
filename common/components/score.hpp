#pragma once

struct Score
{
    int playerScore;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & playerScore;
    }
};
