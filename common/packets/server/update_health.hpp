#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

// Changing this is considered breaking changes.
#pragma pack(push, 1)
struct UpdateHealth
{
    static constexpr auto kId = type::Server::kUpdateHealth;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "UPDATE_HEALTH";

    uint32_t id;
    uint32_t health;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & health;
    }
};
#pragma pack(pop)

}  // namespace packet

