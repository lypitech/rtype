#pragma once

#include "ConnectAck.hpp"
#include "rtnt/Common/Constants.hpp"
#include "rtnt/Core/Packet.hpp"
#include "rtnt/Core/Session.hpp"

namespace rtnt::core::packet::internal {

struct Connect
{
    static constexpr Id kId = static_cast<uint16_t>(SystemMessageId::kConnect);
    static constexpr Flag kFlag = Flag::kReliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("CONNECT");

    template <typename Archive>
    void serialize(Archive&)
    {
    }

    static void onReceive(const std::shared_ptr<Session>& session, const Connect& /*packet*/)
    {
        const ConnectAck response{
            .assignedSessionId = 88  // todo: fix magic number
        };

        session->send(response);
    }
};

}  // namespace rtnt::core::packet::internal
