#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::UserInput
 *
 * @brief Notify the server that the player executed a set of actions.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct UserInput
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Client::kUserInput);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr rtnt::core::packet::Name kName = "USER_INPUT";

    uint8_t input_mask;  ///< The input bitmask of the currently pressed actions.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & input_mask;
    }
};

}  // namespace packet
