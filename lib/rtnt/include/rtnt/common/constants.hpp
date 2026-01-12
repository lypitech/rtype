#pragma once

#define INTERNAL_PACKET_NAME(name) "__rtnt_internal_" name

#include <chrono>
#include <cstdint>
#include <string_view>
#include <vector>

namespace rtnt {

/// @warning    Changing this is considered as a breaking change.
static constexpr uint16_t PROTOCOL_ID = 0x1801;

/// @warning    Changing this is considered as a breaking change.
///
/// todo: Prone to change
static constexpr uint16_t PROTOCOL_VER = 0x0002;

static constexpr uint8_t MAX_RECONNECTION_RETRIES = 3;

static constexpr auto RECONNECTION_TIMEOUT = std::chrono::milliseconds(2000);

namespace core {

using ByteBuffer = std::vector<uint8_t>;

}

namespace core::packet {

static constexpr std::string_view UNKNOWN_PACKET_NAME = "__rtnt_UNKNOWN";

/// todo: Prone to change
static constexpr auto ACK_TIMEOUT = std::chrono::milliseconds(100);

static constexpr auto RESEND_TIMEOUT = std::chrono::milliseconds(200);

static constexpr uint8_t MAX_RESEND_RETRIES = 10;

/**
 * @brief   Internal packet IDs
 * @warning Modifying the order or changing any assigned value is considered as a breaking change.
 */
enum class SystemMessageId : uint16_t
{
    kAck = 0x00,
    kRichAck,
    kConnect,
    kConnectAck,
    kDisconnect,
    kPing,
    kPong,
};

}  // namespace core::packet

}  // namespace rtnt
