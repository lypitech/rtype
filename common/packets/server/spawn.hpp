#pragma once
#include "enums/packets.hpp"
#include "rtnt/Core/Packet.hpp"

namespace packet {

// Changing this is considered breaking changes.
// TODO: add entity attributes.
#pragma pack(push, 1)
/**
 * @struct packet::Spawn
 *
 * @brief Notify the client of the spawn of an entity.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct Spawn
{
    static constexpr auto kId = type::Server::kSpawn;
    static constexpr auto kFlag = rtnt::core::packet::Flag::kUnreliable;
    static constexpr auto kName = "SPAWN";

    uint32_t id;  ///< The id of the spawned entity.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & id;
    }
};
#pragma pack(pop)

}  // namespace packet

