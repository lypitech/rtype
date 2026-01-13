#pragma once

#include "connect_ack.hpp"
#include "rtnt/common/constants.hpp"
#include "rtnt/core/packet.hpp"
#include "rtnt/core/session.hpp"

namespace rtnt::core::packet::internal {

/**
 * @struct  Connect
 * @brief   Initiates the connection handshake (Client -> Server).
 *
 * This is the very first packet sent by a Client when attempting to establish
 * a connection with the Server.
 *
 * When the server receives this packet, it has already created a @code Session@endcode.
 * The @code onReceive@endcode handler accepts the connection by replying with a
 * @code CONNECT_ACK@endcode containing the assigned Session ID.
 */
struct Connect
{
    static constexpr MessageId kId = static_cast<uint16_t>(SystemMessageId::kConnect);
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("CONNECT");

    static void onReceive(const std::shared_ptr<Session>& session,
                          const Connect& /*packet*/)
    {
        const ConnectAck response{.assignedSessionId = session->getId()};

        session->send(response);
    }
};

}  // namespace rtnt::core::packet::internal
