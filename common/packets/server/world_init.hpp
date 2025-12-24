#pragma once
#include "ECS.hpp"
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::WorldInit
 *
 * @brief Send all the information about the current stage to the client.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct WorldInit
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kWorldInit);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "WORLD_INIT";

    uint16_t stage;                              ///< The current started stage.
    std::vector<std::vector<uint8_t>> bitsets;   ///< The bitsets of all the created entities.
    std::vector<rtecs::EntityID> ids;            ///< The ids of all the created entities.
    std::vector<std::vector<uint8_t>> entities;  ///< The content of all created entities.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & stage & bitsets & ids & entities;
    }
};

}  // namespace packet

