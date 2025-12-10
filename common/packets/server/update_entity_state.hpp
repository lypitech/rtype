#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

// Changing this is considered breaking changes.
#pragma pack(push, 1)
/**
 * @struct packet::UpdateEntityState
 *
 * @brief Updates the state of an entity.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct UpdateEntityState
{
    static constexpr auto kId = type::Server::kUpdateEntityState;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr auto kName = "UPDATE_ENTITY_STATE";

    uint32_t id;    ///< The id of the entity to update.
    uint8_t state;  ///< The current state of the entity.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & state;
    }
};
#pragma pack(pop)

}  // namespace packet

