#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

#pragma pack(push, 1)
/**
 * @struct packet::UserInput
 *
 * @brief Notify the server that the player executed a set of actions.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct UserInput
{
    static constexpr packetId kId = static_cast<packetId>(type::Client::kUserInput);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "USER_INPUT";

    uint8_t input_mask;  ///< The input bitmask of the currently pressed actions.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & input_mask;
    }
};
#pragma pack(pop)

}  // namespace packet
