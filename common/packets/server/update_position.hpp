#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::UpdatePosition
 *
 * @brief Updates the position of an entity.
 */
struct UpdatePosition
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kUpdatePosition);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr rtnt::core::packet::Name kName = "UPDATE_POSITION";

    rtecs::types::EntityID id;  ///< The id of the entity
    float_t x;                  //< The x position of the entity
    float_t y;                  //< The y position of the entity
    float_t vx;                 //< The x velocity of the entity (used for dead reckoning)
    float_t vy;                 //< The y velocity of the entity (used for dead reckoning)

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & x & y & vx & vy;
    }
};

}  // namespace packet

