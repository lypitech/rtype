#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

#pragma pack(push, 1)
struct Destroy
{
    static constexpr auto kId = type::Server::kDestroy;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "DESTROY";

    uint32_t id;
    uint32_t earned_points;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & earned_points;
    }
};
#pragma pack(pop)

}  // namespace packet

