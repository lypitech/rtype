#include "logger/Logger.h"
#include "rtnt/Core/Session.hpp"

namespace rt::callback {

void onConnect(std::shared_ptr<rtnt::core::Session>) { LOG_DEBUG("Connected to peer."); }

}  // namespace rt::callback
