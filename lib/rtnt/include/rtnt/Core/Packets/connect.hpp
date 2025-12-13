#pragma once

#include "connect_ack.hpp"
#include "rtnt/Common/constants.hpp"
#include "rtnt/Core/packet.hpp"
#include "rtnt/Core/session.hpp"

namespace rtnt::core::packet::internal {

struct Connect
{
    static constexpr Id kId = static_cast<uint16_t>(SystemMessageId::kConnect);
    static constexpr Flag kFlag = Flag::kReliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("CONNECT");

    static void onReceive(const std::shared_ptr<Session>& session, const Connect& /*packet*/)
    {
        const ConnectAck response{
            .assignedSessionId = session->getId()
        };

        session->send(response);
    }
};

}  // namespace rtnt::core::packet::internal
