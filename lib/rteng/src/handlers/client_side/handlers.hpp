#pragma once
#include <memory>

#include "packets/server/spawn.hpp"
#include "packets/server/update_position.hpp"
#include "packets/server/world_init.hpp"
#include "rtnt/core/session.hpp"

namespace rteng::client_side::handlers {

using SessionPtr = std::shared_ptr<rtnt::core::Session>;

void handleSpawn(const SessionPtr&,
                 const packet::Spawn& packet);
void handleUpdatePosition(const SessionPtr&,
                          const packet::UpdatePosition& packet);
void handleWorldInit(const SessionPtr&,
                     const packet::WorldInit& packet);

}  // namespace rteng::client_side::handlers
