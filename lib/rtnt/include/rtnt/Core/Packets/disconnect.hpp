#pragma once

#include "rtnt/Common/constants.hpp"
#include "rtnt/Core/packet.hpp"

namespace rtnt::core::packet::internal {

struct Disconnect
{
    static constexpr Id kId = static_cast<uint16_t>(SystemMessageId::kDisconnect);
    static constexpr Flag kFlag = Flag::kReliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("DISCONNECT");

    template <typename Archive>
    void serialize(Archive&)
    {
    }

    static void onReceive(const std::shared_ptr<Session>& session, const Disconnect& /*packet*/)
    {
        session->disconnect();
    }
};

}  // namespace rtnt::core::packet::internal
