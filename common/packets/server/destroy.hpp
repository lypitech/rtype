#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::Destroy
 *
 * @brief Notify the client of the destruction of an entity.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct Destroy
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kDestroy);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr rtnt::core::packet::Name kName = "DESTROY";

    uint32_t id;             ///< The id of the destroyed entity.
    uint32_t earned_points;  ///< The points earned for this destruction.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & earned_points;
    }
};

}  // namespace packet

