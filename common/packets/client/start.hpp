#pragma once

#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::Start
 *
 * @brief Ask the server to start the game in the current lobby.
 */
struct Start
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Client::kStart);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr rtnt::core::packet::Name kName = "START";

    template <typename Archive>
    void serialize(Archive&)
    {
        // LALALALLALA J4ENTENDS PAS
    }
};

}  // namespace packet
