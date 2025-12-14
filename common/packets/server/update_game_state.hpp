#pragma once
#include "enums/game_state.hpp"
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

#pragma pack(push, 1)
/**
 * @struct packet::UpdateGameState
 *
 * @brief Updates the state of the game.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct UpdateGameState
{
    static constexpr auto kId = static_cast<packetId>(type::Server::kUpdateGameState);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr std::string kName = "UPDATE_GAME_STATE";

    uint8_t gameState;  ///< The current state of the game.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & gameState;
    }
};
#pragma pack(pop)

}  // namespace packet

