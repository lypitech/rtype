#pragma once

#include <cstdint>

namespace game::state {

static constexpr uint8_t GameMenu = 1;     ///< The game is in the menu
static constexpr uint8_t GameWaiting = 2;  ///< The game is waiting for players to join
static constexpr uint8_t GameLobby = 3;    ///< The game is waiting for players to start
static constexpr uint8_t GameRunning = 4;  ///< The game is running
static constexpr uint8_t GameEnd = 5;      ///< The game is won
static constexpr uint8_t GameOver = 6;     ///< The game is lost

}  // namespace game::state
