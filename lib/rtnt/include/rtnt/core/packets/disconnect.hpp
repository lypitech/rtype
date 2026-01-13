#pragma once

#include "rtnt/common/constants.hpp"
#include "rtnt/core/packet.hpp"

namespace rtnt::core::packet::internal {

/**
 * @struct  Disconnect
 * @brief   Bidirectional session termination signal.
 *
 * This packet can be sent by either the server or the client to gracefully close the connection:
 * - Client -> Server: "cyu im leaving"
 * - Server -> Client: "vasy bouge mtn"
 *
 * The @code onReceive@endcode handler ensures the receiving side immediately marks the session as
 * closed.
 */
struct Disconnect
{
    static constexpr MessageId kId = static_cast<uint16_t>(SystemMessageId::kDisconnect);
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("DISCONNECT");

    static void onReceive(const std::shared_ptr<Session>& session,
                          const Disconnect& /*packet*/)
    {
        LOG_DEBUG("Received DISCONNECT packet, killing session.");
        session->disconnect();
    }
};

}  // namespace rtnt::core::packet::internal
