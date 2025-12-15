#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

/**
 * @struct packet::Spawn
 *
 * @brief Notify the client of the spawn of an entity.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct Spawn
{
    static constexpr auto kId = static_cast<packetId>(type::Server::kSpawn);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr std::string kName = "SPAWN";

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

