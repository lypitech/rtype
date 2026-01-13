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

    void rawSend(Packet& packet,
                 packet::SequenceId sequenceId,
                 packet::OrderId orderId);
    void _internal_sendAck();
    void _internal_disconnect();

    bool isDuplicate(packet::SequenceId sequenceId) const;
    void checkForOldPackets(std::shared_ptr<ByteBuffer> rawData,
                            const packet::Header& header);
};

}  // namespace rtnt::core
