#pragma once
#include "enums/packets.hpp"
#include "rtnt/core/packet.hpp"

namespace packet {

using packetId = rtnt::core::packet::Id;

#pragma pack(push, 1)
/**
 * @struct packet::WorldInit
 *
 * @brief Send all the information about the current stage to the client.
 * @note This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
 */
struct WorldInit
{
    static constexpr auto kId = static_cast<packetId>(type::Server::kWorldInit);
    static constexpr auto kFlag = rtnt::core::packet::Flag::kReliable;
    static constexpr std::string kName = "WORLD_INIT";

    uint16_t stage;                     ///< The current started stage.
    uint32_t size;                      ///< The number of entities to create at initialization.
    std::vector<std::string> entities;  ///< The list of created entities at this point. // TODO: Change type to entity.

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & stage & size & entities;
    }
};
#pragma pack(pop)

}  // namespace packet

