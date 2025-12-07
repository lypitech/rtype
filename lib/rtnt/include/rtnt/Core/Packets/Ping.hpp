#pragma once

#include "rtnt/Core/InternalPackets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace rtnt::core::packet::internal
{

struct PingPacket
{
    static constexpr Id   kId   = static_cast<uint16_t>(SystemMessageId::kPing);
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = "__rtnt_internal_PING";

    uint64_t timestamp;

    template<typename Archive>
    void serialize(Archive& ar)
    {
        ar & timestamp;
    }
};

}
