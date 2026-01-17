#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::LobbyListAck
 *
 * @brief The response of the packet LobbyList.
 */
struct LobbyListAck
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kLobbyListAck);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "LOBBY_LIST_ACK";

    std::vector<uint32_t> roomIds;  ///< The list of rooms' id per page of 20 rooms
    uint32_t page;                  ///< The current page
    uint32_t maxPage;               ///< The max page

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & roomIds & page & maxPage;
    }
};

}  // namespace packet
