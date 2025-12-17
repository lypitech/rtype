#pragma once
#include <memory>

#include "packets/client/user_input.hpp"
#include "rtnt/core/session.hpp"

namespace rteng::server_side::handlers {

using SessionPtr = std::shared_ptr<rtnt::core::Session>;

void handleUserInput(const SessionPtr&,
                     const packet::UserInput& packet);

}  // namespace rteng::server_side::handlers
