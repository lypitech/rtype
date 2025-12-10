#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

#pragma pack(push, 1)
/**
 * @struct packet::UserInput
 *
 * @brief Notify the server that the player executed a set of actions.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct UserInput
{
    static constexpr auto kId = type::Client::kUserInput;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "USER_INPUT";

    uint8_t input_mask;  ///< The input mask of the currently pressed actions.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & input_mask;
    }
};
#pragma pack(pop)

}  // namespace packet
