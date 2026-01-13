#pragma once
#include "enums/game_state.hpp"
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::UpdateGameState
 *
 * @brief Updates the state of the game.
 */
struct UpdateGameState
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kUpdateGameState);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "UPDATE_GAME_STATE";

    uint8_t gameState;  ///< The current state of the game.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & gameState;
    }
};

}  // namespace packet

