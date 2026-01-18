#pragma once

#include <cstdint>

namespace entity::state {

static constexpr uint8_t PlayerWaiting = 1;  ///< The player waiting for players to join
static constexpr uint8_t PlayerReady = 2;    ///< The player is ready to start the game
static constexpr uint8_t EntityAlive = 3;    ///< The entity is alive
static constexpr uint8_t EntityDead = 4;     ///< The entity is dead

}  // namespace entity::state
