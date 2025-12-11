#pragma once

namespace game {

/**
 * @class State
 *
 * @brief The possible states of a game.
 */
enum class State
{
    kGameOver = 0x01,  ///< The game is lost.
    kGameStart,        ///< Start the game.
    kGameEnd,          ///< The game is won.
};

}  // namespace game
