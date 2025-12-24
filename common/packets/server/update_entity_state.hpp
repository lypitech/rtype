#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::UpdateEntityState
 *
 * @brief Updates the state of an entity.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct UpdateEntityState
{
    static constexpr auto kId =
        static_cast<rtnt::core::packet::Id>(type::Server::kUpdateEntityState);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "UPDATE_ENTITY_STATE";

    uint32_t id;    ///< The id of the entity to update.
    uint8_t state;  ///< The current state of the entity.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & state;
    }
};

}  // namespace packet

