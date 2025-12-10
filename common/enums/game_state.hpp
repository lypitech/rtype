#pragma once

namespace game {

enum class State
{
    kGameOver = 1,   // The game is lost.
    kGameStart = 2,  // Start the game.
    kGameEnd = 3,    // The game is won.
};

}  // namespace game
