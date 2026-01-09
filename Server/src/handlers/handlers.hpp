#pragma once

#include "handlers.hpp"
#include "packets/client/user_input.hpp"

using SessionPtr = std::shared_ptr<rtnt::core::Session>;

namespace packet::handler {

/**
 * @brief Handles a user input
 * @param packet a copy of a UserInput packet
 */
void handleUserInput(Lobby& lobby,
                     const UserInput& packet);

}  // namespace packet::handler
