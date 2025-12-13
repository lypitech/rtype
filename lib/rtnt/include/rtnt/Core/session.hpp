#pragma once

#include <asio/ip/udp.hpp>
#include <chrono>

#include "packet.hpp"

namespace rtnt::core {

using asio::ip::udp;
using namespace std::chrono;

namespace session {

using Id = uint32_t;

}

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
    explicit Session(udp::endpoint endpoint, SendToPeerFunction sendToPeerFunction);

    /**
     * @brief   Processes incoming raw data and attempts to construct a valid Packet.
     *
     * This method performs several checks (in order):
     * - Buffer size check (must be >= header size)
     * - Protocol ID check (security)
     * - RUDP Sequence update
     *
     * @param   rawData     The raw buffer received from the Peer
     * @param   outPacket   The clean packet to fill if parsing is successful
     * @return  @code true@endcode if the packet is valid and should be handled by the user.
     * @return  @code false@endcode if the packet is either random internet noise, coming from an outdated Peer,
     * corrupted or invalid.
     */
    bool handleIncoming(std::shared_ptr<ByteBuffer> rawData, Packet& outPacket);

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

    void disconnect();

    [[nodiscard]] session::Id getId() const { return _id; }

    /**
     * @return  The endpoint associated with this Session (the remote Peer).
     */
    [[nodiscard]] udp::endpoint getEndpoint() const { return _endpoint; }

    /**
     * @return The timestamp of the last valid packet received from this session.
     * Used by the Server to timeout inactive clients.
     */
    [[nodiscard]] time_point<steady_clock> getLastSeenTimestamp() const { return _lastSeen; }

    [[nodiscard]] bool shouldClose() const { return _shouldClose; }

private:
    const session::Id _id;

    const udp::endpoint _endpoint;
    SendToPeerFunction _sendToPeerFunction;

    // RUDP state
    uint32_t _localSequenceId = 0;
    uint32_t _remoteSequenceId = 0;

    time_point<steady_clock> _lastSeen;
    bool _shouldClose;
};

}  // namespace rtnt::core
