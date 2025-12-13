#pragma once

#include "rtnt/common/constants.hpp"
#include "rtnt/core/packet.hpp"

namespace rtnt::core::packet::internal {

struct Disconnect
{
    static constexpr Id kId = static_cast<uint16_t>(SystemMessageId::kDisconnect);
    static constexpr Flag kFlag = Flag::kReliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("DISCONNECT");

    static void onReceive(const std::shared_ptr<Session>& session, const Disconnect& /*packet*/)
    {
        LOG_DEBUG("Received DISCONNECT packet, killing session.");
        session->disconnect();
    }
};

}  // namespace rtnt::core::packet::internal
