#pragma once

#include "rtnt/common/constants.hpp"
#include "rtnt/core/packet.hpp"

namespace rtnt::core::packet::internal {

/**
 * @struct  ConnectAck
 * @brief   Finalizes the connection handshake (Server -> Client).
 *
 * This packet is sent by the server in response to a valid @code CONNECT@endcode packet.
 * It carries the authoritative @code session::Id@endcode assigned by the server.
 *
 * When received by the client, the handshake is considered complete.
 */
struct ConnectAck
{
    static constexpr Id kId = static_cast<uint16_t>(SystemMessageId::kConnectAck);
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("CONNECT_ACK");

    uint32_t assignedSessionId;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & assignedSessionId;
    }
};

}  // namespace rtnt::core::packet::internal
