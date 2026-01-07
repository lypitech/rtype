#pragma once

#define INTERNAL_PACKET_NAME(name) "__rtnt_internal_" name

#include <cstdint>
#include <string_view>
#include <vector>

namespace rtnt {

/// @warning    Changing this is considered as a breaking change.
static constexpr uint16_t PROTOCOL_ID = 0x1801;

/// @warning    Changing this is considered as a breaking change.
///
/// todo: Prone to change
static constexpr uint16_t PROTOCOL_VER = 0x0001;

namespace core {

using ByteBuffer = std::vector<uint8_t>;

}

namespace core::packet {

static constexpr std::string_view UNKNOWN_PACKET_NAME = "__rtnt_UNKNOWN";

/**
 * @brief   Internal packet IDs
 * @warning Modifying the order or changing any assigned value is considered as a breaking change.
 */
enum class SystemMessageId : uint16_t
{
    kAck = 0x00,
    kConnect,
    kConnectAck,
    kDisconnect,
    kPing,
    kPong,
};

}  // namespace core::packet

}  // namespace rtnt
