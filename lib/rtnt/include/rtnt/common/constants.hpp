#pragma once

#define INTERNAL_PACKET_NAME(name) "__rtnt_internal_" name

#include <chrono>
#include <cstdint>
#include <string_view>
#include <vector>

namespace rtnt {

/// @brief      Unique ID of @code rtntp@endcode.
/// @warning    Changing this is considered as a breaking change.
static constexpr uint16_t PROTOCOL_ID = 0x2204;

/// @brief      Version of the protocol (@code rtntp@endcode).
/// @warning    Changing this is considered as a breaking change.
///
/// todo: Prone to change
static constexpr uint16_t PROTOCOL_VER = 0x0002;

/// @brief  Maximum number of times a client will attempt a connection to a remote server. After
/// reaching that number, it will just give up.
static constexpr uint8_t MAX_RECONNECTION_ATTEMPTS = 3;

/// @brief  Amount of time between each reconnection attempt.
static constexpr auto RECONNECTION_TIMEOUT = std::chrono::milliseconds(2000);

namespace core {

using ByteBuffer = std::vector<uint8_t>;

}

namespace core::packet {

static constexpr std::string_view UNKNOWN_PACKET_NAME = "__rtnt_UNKNOWN";

/// @brief  Amount of time a packet will be considered not acknowledged by the remote peer.
/// If reached, the packet will automatically be resent.
///
/// todo: Prone to change
static constexpr auto ACK_TIMEOUT = std::chrono::milliseconds(100);

/// @brief  Amount of time between each packet resend.
static constexpr auto RESEND_TIMEOUT = std::chrono::milliseconds(200);

/// @brief  Maximum number of times a peer will attempt to resend a packet. If reached, the
/// connection will be considered as dead.
static constexpr uint8_t MAX_RESEND_ATTEMPTS =
#if defined(RTNT_TESTS)
    1 << 7;
#else
    1 << 3
#endif
;

/// @brief  Maximum number of packet IDs that can be stored in the missing packet history (in
/// addition to acknowledge bitfield).
static constexpr size_t MAX_PACKET_HISTORY_SIZE = 1 << 6;

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
