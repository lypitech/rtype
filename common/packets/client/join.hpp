#pragma once

#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

struct Join
{
    static constexpr auto kId = type::Client::kJoin;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "JOIN";

    std::string username;
    uint8_t room_id;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & username & room_id;
    }
};

}  // namespace packet
