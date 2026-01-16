#pragma once
#include "enums/packets.hpp"
#include "rtecs/ECS.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::WorldInit
 *
 * @brief Send all the information about the current stage to the client.
 */
struct WorldInit
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kWorldInit);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "WORLD_INIT";

    uint8_t state;                               ///< The current state of the game.
    std::vector<std::vector<uint64_t>> bitsets;  ///< The bitsets of all the created entities.
    std::vector<rtecs::types::EntityID> ids;     ///< The ids of all the created entities.
    std::vector<std::vector<uint8_t>> entities;  ///< The content of all created entities.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & state & bitsets & ids & entities;
    }
};

}  // namespace packet

