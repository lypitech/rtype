#include "logger/Logger.h"
#include "rtnt/Core/Packet.hpp"

namespace rt::callback {

void onMessage(rtnt::core::Packet&) { LOG_DEBUG("He talked to me !"); }

}  // namespace rt::callback
