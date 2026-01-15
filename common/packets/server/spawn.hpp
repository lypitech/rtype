#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

/**
 * @struct packet::Spawn
 *
 * @brief Notify the client of the spawn of an entity.
 */
struct Spawn
{
    static constexpr auto kId = static_cast<rtnt::core::packet::Id>(type::Server::kSpawn);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr rtnt::core::packet::Name kName = "SPAWN";

    uint32_t id;                   ///< The id of the entity for future reference
    std::vector<uint8_t> bitmask;  ///< The index of the activated bytes.
    std::vector<uint8_t> content;  ///< The content of the components.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id & bitmask & content;
    }
};

}  // namespace packet

