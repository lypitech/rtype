#pragma once

#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::JoinAck
 *
 * @brief Notify the client about the status of a @code join@endcode request.
 */
struct JoinAck
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kJoinAck);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "JOIN_ACK";

    rtecs::types::EntityID id;  ///< The id of the entity assigned to the player.
    uint16_t roomId;
    uint64_t gameState;  ///< The current state of the game.
    uint8_t status;      ///< The status of the request, a boolean.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & roomId & gameState & status;
    }
};

}  // namespace packet

