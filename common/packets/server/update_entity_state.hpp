#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

// Changing this is considered breaking changes.
#pragma pack(push, 1)
struct UpdateEntityState
{
    static constexpr auto kId = type::Server::kUpdateEntityState;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr auto kName = "UPDATE_ENTITY_STATE";

    uint32_t id;
    uint8_t state;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & state;
    }
};
#pragma pack(pop)

}  // namespace packet

