#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

#pragma pack(push, 1)
/**
 * @struct packet::JoinAck
 *
 * @brief Notify the client about the status of a @code join@endcode request.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct JoinAck
{
    static constexpr auto kId = static_cast<packetId>(type::Server::kJoinAck);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr std::string kName = "JOIN_ACK";

    uint32_t id;     ///< The id of the entity assigned to the player.
    uint8_t status;  ///< The status of the request, a boolean.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & status;
    }
};
#pragma pack(pop)

}  // namespace packet

