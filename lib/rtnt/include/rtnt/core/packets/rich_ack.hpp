#pragma once

#include "rtnt/common/constants.hpp"
#include "rtnt/core/packet.hpp"

namespace rtnt::core::packet::internal {

/**
 * @struct  RichAck
 * @brief   Internal packet used for out-of-band (OOB) acknowledgments.
 *
 * Standard RUDP ACKs rely on a 32-bit bitfield in the packet header to acknowledge the last 32
 * packets relative to the highest received ID.
 *
 * However, in high packet-loss situations, a packet might arrive very late, falling outside this
 * 32-bit window. Since the header cannot represent this, the sender would normally keep
 * retransmitting it forever.
 *
 * The @code RICH_ACK@endcode solves this by carrying an explicit list of these "old" Sequence IDs
 * in its payload. When the sender receives this, it marks those specific IDs as received, stopping
 * the retransmission loop.
 */
struct RichAck
{
    static constexpr Id kId = static_cast<uint16_t>(SystemMessageId::kRichAck);
    static constexpr ChannelId kChannel = INTERNAL_CHANNEL_ID;
    static constexpr Flag kFlag = Flag::kUnreliable;
    static constexpr Name kName = INTERNAL_PACKET_NAME("RICH_ACK");

    std::deque<uint32_t> oobAcks;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & oobAcks;
    }
};

}  // namespace rtnt::core::packet::internal
