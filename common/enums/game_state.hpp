#pragma once

#include <cstdint>

namespace game::state {

static constexpr uint8_t GameMenu = 1;     ///< The game is in the menu
static constexpr uint8_t GameLobby = 2;    ///< The game is waiting for players to join
static constexpr uint8_t GameRunning = 3;  ///< The game is running
static constexpr uint8_t GameEnd = 4;      ///< The game is won
static constexpr uint8_t GameOver = 5;     ///< The game is lost

}  // namespace game::state
