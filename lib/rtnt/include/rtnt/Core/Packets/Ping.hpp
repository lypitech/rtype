#pragma once

#include "rtnt/Common/Constants.hpp"
#include "rtnt/Core/Packet.hpp"

namespace rtnt::core::packet::internal
{

struct PingPacket
{
    static constexpr Id   kId   = static_cast<uint16_t>(SystemMessageId::kPing);
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("PING");

    uint64_t timestamp;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & timestamp;
    }
};

}
