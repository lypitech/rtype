#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

struct UserInput
{
    static constexpr auto kId = type::Client::kUserInput;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "USER_INPUT";

    uint8_t input_mask;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & input_mask;
    }
};

}  // namespace packet
