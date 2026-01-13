#pragma once

#include "rtnt/common/constants.hpp"
#include "rtnt/core/packet.hpp"

namespace rtnt::core::packet::internal {

struct RichAck
{
    static constexpr MessageId kId = static_cast<uint16_t>(SystemMessageId::kRichAck);
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("RICH_ACK");

    std::deque<uint32_t> oobAcks;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & oobAcks;
    }
};

}  // namespace rtnt::core::packet::internal
