#pragma once

#include "rtnt/Common/Constants.hpp"
#include "rtnt/Core/Packet.hpp"

namespace rtnt::core::packet::internal
{

struct PongPacket
{
    static constexpr Id   kId   = static_cast<uint16_t>(SystemMessageId::kPong);
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("PONG");

    uint64_t timestamp;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & timestamp;
    }
};

}
