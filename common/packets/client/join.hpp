#pragma once

#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::Join
 *
 * @brief Ask to join a game.
 */
struct Join
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Client::kJoin);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr rtnt::core::packet::Name kName = "JOIN";

    std::string username;  ///< The username of the joining player.
    uint32_t room_id;      ///< The id of the room to join. 0 for any.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & username & room_id;
    }
};

}  // namespace packet
