#pragma once

#include "handlers.hpp"
#include "packets/client/user_input.hpp"

using SessionPtr = std::shared_ptr<rtnt::core::Session>;

class Lobby;

namespace packet::handler {

/**
 * @brief Handles a user input.
 * @param session A reference to the session pointer of the user.
 * @param lobby A reference to the lobby of the user.
 * @param packet A copy of a UserInput packet.
 */
void handleUserInput(const SessionPtr& session,
                     Lobby& lobby,
                     const UserInput& packet);

}  // namespace packet::handler
