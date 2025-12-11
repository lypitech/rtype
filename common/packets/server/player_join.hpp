#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

#pragma pack(push, 1)
/**
 * @struct packet::PlayerJoin
 *
 * @brief Notify a client about a joining player.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct PlayerJoin
{
    static constexpr auto kId = static_cast<packetId>(type::Server::kPlayerJoin);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr std::string kName = "PLAYER_JOIN";

    uint32_t id;     ///< The id assigned to this entity.
    uint8_t team;    ///< The team assigned to this player.
    uint8_t status;  ///< The status of this player.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & status;
    }
};
#pragma pack(pop)

}  // namespace packet

