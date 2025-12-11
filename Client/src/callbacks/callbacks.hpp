#pragma once
#include "rtnt/Core/packet.hpp"

namespace rt::callback {

void onConnect();
void onDisconnect();
void onMessage(rtnt::core::Packet&);

}  // namespace rt::callback

