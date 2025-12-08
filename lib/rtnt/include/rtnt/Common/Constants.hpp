#pragma once

#include <cstdint>

namespace rtnt
{

/// @warning    Changing this is considered a breaking change.
///
/// todo: Prone to change
static constexpr uint16_t PROTOCOL_ID = 0xfeed;

/// @warning    Changing this is considered a breaking change.
///
/// todo: Prone to change
static constexpr uint16_t PROTOCOL_VER = 0x0001;

namespace core::packet
{

    enum class SystemMessageId : uint16_t
    {
        kConnect = 0x01,
        kConnectAck,
        kPing,
        kPong,
    };

}

}
