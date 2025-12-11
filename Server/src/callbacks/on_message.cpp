#include "logger/Logger.h"
#include "rtnt/Core/packet.hpp"

namespace rt::callback {

void onMessage(std::shared_ptr<rtnt::core::Session>, rtnt::core::Packet& p)
{
    LOG_DEBUG("Received packet with id {}.", p.getId());
}

}  // namespace rt::callback
