#include "logger/Logger.h"
#include "rtnt/Core/Session.hpp"

namespace rt::callback {

void onDisconnect(std::shared_ptr<rtnt::core::Session>) { LOG_DEBUG("Disconnected from peer."); }

}  // namespace rt::callback
