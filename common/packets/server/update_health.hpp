#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

/**
 * @struct packet::UpdateHealth
 *
 * @brief Update the health of an entity.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct UpdateHealth
{
    static constexpr auto kId = static_cast<packetId>(type::Server::kUpdateHealth);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr std::string kName = "UPDATE_HEALTH";

    uint32_t id;
    uint32_t health;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & health;
    }
};

}  // namespace packet

