#pragma once

#include <asio/ip/udp.hpp>
#include <chrono>
#include <map>

#include "packet.hpp"

namespace rtnt::core {

using asio::ip::udp;
using namespace std::chrono;

namespace session {

using Id = uint32_t;

}

struct SentPacketInfo final
{
    Packet packet;
    time_point<steady_clock> sentTime;
    packet::SequenceId sequenceId = 0;
    packet::OrderId orderId = 0;
    uint8_t retries =
        0;  // fixme: Careful because if the maximum limit is greater than this, then on est foutus
};

/**
 * @class   Session
 * @brief   Basically a logical connection with a remote peer.
 *
 * The Session is the "Brain" of the connection. It acts as a translation layer:
 * - INCOMING: Raw bytes -> Validation -> Header stripping -> Packet Object
 * - OUTGOING: Packet object -> Header attachment -> Sequencing -> Raw Bytes
 * It manages RUDP state (Sequence IDs, ACKs, Ordering buffers).
 */
class Session
{
    using SendToPeerFunction = std::function<void(std::shared_ptr<ByteBuffer>)>;

public:
    explicit Session(udp::endpoint endpoint,
                     SendToPeerFunction sendToPeerFunction);

    /**
     * @brief   Processes incoming raw data and returns a list of valid packets.
     *
     * This method performs several checks (in order):
     * - Buffer size check (must be >= header size)
     * - Protocol ID check (security)
     * - RUDP Sequence update
     *
     * @param   rawData     The raw buffer received from the Peer
     * @returns Vector of valid packets that should be handled by the user.
     */
    std::vector<Packet> handleIncoming(std::shared_ptr<ByteBuffer> rawData);

    template <typename T>
    void send(const T& packetData)
    {
        packet::verifyPacketData<T>();

        LOG_DEBUG("Session sending packet #{} {}...", T::kId, packet::getName<T>());

        Packet packet(T::kId, packet::getFlag<T>());
        packet << packetData;
        send(packet);
    }

    /**
     * @brief   Encapsulates a user-defined Packet into a protocol frame (RUDP) and sends it.
     *
     * This attaches the header (Sequence ID, ACK ID, etc.) and gives the result to the Peer for network transmission.
     */
    void send(Packet& packet);

    /**
     * @brief   Applies RUDP logic: reliable packets (resend logic) etc.
     */
    void update();

    /**
     * @brief   Marks the session as closed.
     * @note    This function does NOT remove the session from anywhere. It is up to the Peer child to do this.
     */
    void disconnect();

    /**
     * @return  Session's unique ID.
     * @note    By default, ID is equal to 0. It is overwritten when handshake with the remote client has been done.
     */
    [[nodiscard]] session::Id getId() const { return _id; }

    /**
     * @return  The endpoint associated with this Session (the remote Peer).
     */
    [[nodiscard]] udp::endpoint getEndpoint() const { return _endpoint; }

    /**
     * @return The timestamp of the last valid packet received from this session.
     * Used by the Server to timeout inactive clients.
     */
    [[nodiscard]] time_point<steady_clock> getLastSeenTimestamp() const
    {
        std::lock_guard lock(_mutex);
        return _lastSeen;
    }

    /**
     * @return  Whether the session should be closed or not.
     */
    [[nodiscard]] bool shouldClose() const { return _shouldClose; }

private:
    const session::Id _id;

    const udp::endpoint _endpoint;
    SendToPeerFunction _sendToPeerFunction;

    // RUDP state //
    bool _hasReceivedRemotePacket = false;

    packet::SequenceId _localSequenceId = 0;
    packet::SequenceId _remoteSequenceId = 0;

    packet::AcknowledgeId _remoteAcknowledgeId = 0;
    packet::AcknowledgeBitfield _remoteAcknowledgeBitfield = 0;

    packet::OrderId _localOrderId = 0;
    packet::OrderId _nextExpectedOrderId = 0;

    std::map<packet::OrderId, Packet> _reorderBuffer;
    std::map<packet::SequenceId, SentPacketInfo> _sentPackets;
    std::deque<packet::AcknowledgeId> _oldPacketHistory;
    mutable std::mutex _mutex;

    bool _hasUnsentAck = false;
    time_point<steady_clock> _lastAckTime;
    // ---------- //

    time_point<steady_clock> _lastSeen;
    bool _shouldClose = false;

    /**
     * @brief   Constructs the final wire-format buffer and transmits it to the Peer.
     *
     * This is the lowest-level sending function in the session.
     * It performs the following:
     * - Header construction: Populates the @code packet::Header@endcode fields.
     * - ACK Piggybacking: Attaches the current @code _remoteAcknowledgeId@endcode and bitfield to
     *                     the header, ensuring that every outgoing packet helps acknowledge
     *                     received data.
     *                     Sets the @code kHasAck@endcode flag if valid ACK data is present.
     * - Serialization: Combines the header and the packet payload into a contiguous
     *                  @code ByteBuffer@endcode, handling network byte order conversion.
     * - Transmission: Invokes the @code _sendToPeerFunction@endcode to hand the buffer off to the
     *                 network socket.
     *
     * @param   packet      The high-level packet object containing metadata and payload
     * @param   sequenceId  The assigned Sequence ID for this frame
     * @param   orderId     The assigned Order ID for this frame (0 if unordered)
     */
    void rawSend(Packet& packet,
                 packet::SequenceId sequenceId,
                 packet::OrderId orderId);

    /**
     * @brief   Updates the local RUDP tracking state (which we call the Sliding Window) based on a
     *          received Sequence ID.
     *
     * This function manages the head (@code _remoteAcknowledgeId@endcode) and the tail
     * (the bitfield) of the reception window.
     *
     * It handles three eventualities:
     * - New head: The received ID is newer than the current head. The bitfield is shifted left.
     *             Any set bits that "fall off" the left side of the 32-bit window during the shift
     *             are moved into the @code _oldPacketHistory@endcode to maintain replay protection
     *             for older packets.
     * - Inside window: The received ID is older than head but in the 32-cell window.
     *                  The corresponding bit in the bitfield is set.
     * - Out of window: The received ID is too old for the bitfield. It is added directly to
     *                  @code _oldPacketHistory@endcode.
     *
     * @param   sequenceId  The Sequence ID of the packet just received
     */
    void updateAcknowledgeInfo(packet::SequenceId sequenceId);

    /**
     * @brief   Constructs and sends an acknowledgement packet (@code ACK@endcode or
     *          @code RICH_ACK@endcode) to the remote peer.
     *
     * This function decides which type of ACK to send based on the current state:
     * - Standard ACK: If @code _oldPacketHistory@endcode is empty, sends a lightweight header-only
     *                 packet containing just the highest ID and the 32-bit bitfield.
     * - Rich ACK: If @code _oldPacketHistory@endcode contains data, sends a @code RICH_ACK@endcode
     *             packet containing the history in the payload.
     */
    void _internal_sendAck();

    /**
     * @brief   Marks the session for closure.
     *
     * @note    This function expects the caller to hold @code _mutex@endcode.
     * @warning This function does not close the session.
     *          It only marks it, like "this session must be closed as soon as possible".
     */
    void _internal_disconnect();

    /**
     * @brief   Checks if a packet has already been received to prevent replay attacks or redundant
     *          processing.
     *
     * 3 verifications are done:
     * - Is it equal to the last received packet?
     * - If within the last 32 packets, is the bit set in the bitfield?
     * - If older than 32 packets, is it present in the @code _oldPacketHistory@endcode buffer?
     *
     * @param   sequenceId  The sequence ID to check
     * @return  @code true@endcode if the packet is a duplicate, @code false@endcode otherwise.
     */
    bool isDuplicate(packet::SequenceId sequenceId) const;

    /**
     * @brief   Parses the payload of a @code kRichAck@endcode packet.
     *
     * This function deserializes that list from the payload and removes the corresponding packets
     * from the local @code _sentPackets@endcode buffer.
     *
     * @see     rtnt::core::packet::internal::RichAck
     * @param   rawData The raw buffer containing the packet payload
     * @param   header  The parsed header of the packet
     */
    void checkForOldPackets(std::shared_ptr<ByteBuffer> rawData,
                            const packet::Header& header);
};

}  // namespace rtnt::core
