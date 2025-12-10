#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

struct JoinAck
{
    static constexpr auto kId = type::Server::kJoinAck;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr auto kName = "JOIN_ACK";

    uint32_t id;
    uint8_t status;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & status;
    }
};

}  // namespace packet

