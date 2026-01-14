#pragma once

#include "rtnt/Common/constants.hpp"
#include "rtnt/core/packet.hpp"

namespace rtnt::core::packet::internal {

struct Ping
{
    static constexpr Id kId = static_cast<uint16_t>(SystemMessageId::kPing);
    static constexpr ChannelId kChannel = INTERNAL_CHANNEL_ID;
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("PING");

    uint64_t timestamp;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & timestamp;
    }
};

}  // namespace rtnt::core::packet::internal
