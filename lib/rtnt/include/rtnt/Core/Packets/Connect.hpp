#pragma once

#include "rtnt/Common/Constants.hpp"
#include "rtnt/Core/Packet.hpp"

namespace rtnt::core::packet::internal
{

struct Connect
{
    static constexpr Id   kId   = static_cast<uint16_t>(SystemMessageId::kConnect);
    static constexpr Flag kFlag = Flag::kReliable;
    static constexpr Name kName = "__rtnt_internal_CONNECT";

    uint16_t protocolVer = PROTOCOL_VER;

    template<typename Archive>
    void serialize(Archive& ar)
    {
        ar & protocolVer;
    }
};

}
