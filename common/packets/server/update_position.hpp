#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

#pragma pack(push, 1)
/**
 * @struct packet::UpdatePosition
 *
 * @brief Updates the position of an entity.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct UpdatePosition
{
    static constexpr auto kId = static_cast<packetId>(type::Server::kUpdatePosition);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr std::string kName = "UPDATE_POSITION";

    uint32_t id;          ///< The id of the entity
    uint16_t position_x;  //< The x position of the entity
    uint16_t position_y;  //< The y position of the entity
    uint16_t velocity_x;  //< The x velocity of the entity (used for dead reckoning)
    uint16_t velocity_y;  //< The y velocity of the entity (used for dead reckoning)

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & position_x & position_y & velocity_x & velocity_y;
    }
};
#pragma pack(pop)

}  // namespace packet

