#pragma once
#include "rtnt/Core/packet.hpp"

namespace rt::callback {

void onConnect(std::shared_ptr<rtnt::core::Session>);
void onDisconnect(std::shared_ptr<rtnt::core::Session>);
void onMessage(std::shared_ptr<rtnt::core::Session>, rtnt::core::Packet&);

}  // namespace rt::callback

