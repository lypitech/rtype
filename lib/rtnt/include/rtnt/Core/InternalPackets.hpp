#pragma once

#include <cstdint>

namespace rtnt::core
{

enum class SystemMessageId : uint16_t
{
    kConnect = 0x01,
    kConnectAck,
    kPing,
    kPong,
};

}
