#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::LobbyList
 *
 * @brief Ask the server for a list of lobby.
 */
struct LobbyList
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kLobbyList);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "LOBBY_LIST";

    uint32_t page;  ///< The current page

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & page;
    }
};

}  // namespace packet

