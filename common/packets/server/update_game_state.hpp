#pragma once
#include "enums/game_state.hpp"
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

// Changing this is considered breaking changes.
struct UpdateGameState
{
    static constexpr auto kId = type::Server::kUpdateGameState;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr auto kName = "UPDATE_GAME_STATE";

    uint32_t id;
    uint8_t gameState;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & gameState;
    }
};

}  // namespace packet

