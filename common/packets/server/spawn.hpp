#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

// Changing this is considered breaking changes.
// TODO: add entity attributes.
struct Spawn
{
    static constexpr auto kId = type::Server::kSpawn;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "SPAWN";

    uint32_t id;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id;
    }
};

}  // namespace packet

