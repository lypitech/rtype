#pragma once

#include <cstdint>

namespace player::state {

static constexpr uint8_t PlayerWaiting = 1;  ///< The player waiting for players to join
static constexpr uint8_t PlayerReady = 2;    ///< The player is ready to start the game
static constexpr uint8_t PlayerAlive = 2;    ///< The player is alive
static constexpr uint8_t PlayerDead = 2;     ///< The player is dead

}  // namespace player::state
