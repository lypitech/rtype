#pragma once
#include <memory>

#include "packets/server/spawn.hpp"
#include "packets/server/update_position.hpp"
#include "packets/server/world_init.hpp"
#include "rtnt/core/session.hpp"

namespace client {
using SessionPtr = std::shared_ptr<rtnt::core::Session>;
struct HandlerToolbox;
}  // namespace client

namespace packet::handler {

void handleSpawn(const Spawn& packet,
                 client::HandlerToolbox& toolbox);
// void handleUpdatePosition(const SessionPtr&,
//                           const packet::UpdatePosition& packet);
// void handleWorldInit(const SessionPtr&,
//                      const packet::WorldInit& packet);

}  // namespace packet::handler
