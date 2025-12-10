#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

// Changing this is considered breaking changes.
struct UpdateGameState
{
    static constexpr auto kId = type::Server::kWorldInit;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr auto kName = "WORLD_INIT";

    uint32_t id;
    uint32_t size;
    std::vector<std::string> entities;  // TODO: Change type to entity.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & size & entities;
    }
};

}  // namespace packet

