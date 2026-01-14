#pragma once

#include <cstdint>

namespace menu::state {

static constexpr uint8_t MenuHome = 1;      ///< The client is in the main menu
static constexpr uint8_t MenuSettings = 2;  ///< The client is configuring its keybinds, ...
static constexpr uint8_t MenuJoin = 3;      ///< The client is entering host and port of the server
static constexpr uint8_t MenuLobby =
    4;  ///< The client has joined a game but is waiting for other players

}  // namespace menu::state
