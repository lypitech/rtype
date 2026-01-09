#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::PlayerJoin
 *
 * @brief Notify a client about a joining player.
 */
struct PlayerJoin
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kPlayerJoin);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "PLAYER_JOIN";

    uint32_t id;     ///< The id assigned to this entity.
    uint8_t team;    ///< The team assigned to this player.
    uint8_t status;  ///< The status of this player.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & status;
    }
};

}  // namespace packet

