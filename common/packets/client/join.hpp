#pragma once

#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

#pragma pack(push, 1)  // forcing strict alignment, no compiler padding
/**
 * @struct packet::Join
 *
 * @brief Ask to join a game.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct Join
{
    static constexpr auto kId = type::Client::kJoin;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "JOIN";

    std::string username;  ///< The username of the joining player.
    uint8_t room_id;       ///< The id of the room to join. 0 for any.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & username & room_id;
    }
};
#pragma pack(pop)

}  // namespace packet
