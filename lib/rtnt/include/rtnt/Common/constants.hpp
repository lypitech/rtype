#pragma once

#define INTERNAL_PACKET_NAME(name) "__rtnt_internal_" name

#include <cstdint>

namespace rtnt {

/// @warning    Changing this is considered as a breaking change.
static constexpr uint16_t PROTOCOL_ID = 0x1801;

/// @warning    Changing this is considered as a breaking change.
///
/// todo: Prone to change
static constexpr uint16_t PROTOCOL_VER = 0x0001;

namespace core::packet {

static constexpr std::string_view UNKNOWN_PACKET_NAME = "__rtnt_UNKNOWN";

enum class SystemMessageId : uint16_t
{
    kConnect = 0x01,  /// @warning   Changing this value is considered as a breaking change.
    kConnectAck,
    kPing,
    kPong,
};

}  // namespace core::packet

}  // namespace rtnt
