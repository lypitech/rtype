#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

// Changing this is considered breaking changes.
#pragma pack(push, 1)
struct UpdatePosition
{
    static constexpr auto kId = type::Server::kUpdatePosition;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "UPDATE_POSITION";

    uint32_t id;
    uint16_t position_x;
    uint16_t position_y;
    uint16_t velocity_x;
    uint16_t velocity_y;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & position_x & position_y & velocity_x & velocity_y;
    }
};
#pragma pack(pop)

}  // namespace packet

