#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::UpdateHealth
 *
 * @brief Update the health of an entity.
 */
struct UpdateHealth
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kUpdateHealth);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr rtnt::core::packet::Name kName = "UPDATE_HEALTH";

    rtecs::types::EntityID id;
    uint32_t health;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & health;
    }
};

}  // namespace packet

