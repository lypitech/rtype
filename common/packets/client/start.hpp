#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

struct Start
{
    static constexpr auto kId = type::Client::kStart;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "START";

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar;
    }
};

}  // namespace packet
