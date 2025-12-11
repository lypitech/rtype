#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

#pragma pack(push, 1)
/**
 * @struct packet::Start
 *
 * @brief Ask the server to start the game in the current lobby.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct Start
{
    static constexpr packetId kId = static_cast<packetId>(type::Client::kStart);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "START";

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar;
    }
};
#pragma pack(pop)

}  // namespace packet
