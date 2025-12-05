#pragma once

#include <chrono>
#include <asio/ip/udp.hpp>

#include "Packet.hpp"

namespace rtnt::core
{

using asio::ip::udp;
using namespace std::chrono;

class Session
{

using SendToPeerFunction = std::function<void(const ByteBuffer&)>;

public:
    explicit Session(
        udp::endpoint endpoint,
        SendToPeerFunction sendToPeerFunction
    );

    /**
     * @brief   Processes raw bytes coming from the Peer.
     *
     * @param   rawData     The raw buffer received from the network (coming from the Peer)
     * @param   outPacket   The clean packet to fill if parsing is successful
     * @return  @code true@endcode if the packet is valid and should be handled by the user.
     */
    bool handleIncoming(
        const ByteBuffer& rawData,
        Packet& outPacket
    );

    /**
     * @brief   Sends a packet to the network (will pass bytes to the Peer).
     *
     * @param   packet  Packet to send
     */
    void send(Packet& packet);

    [[nodiscard]] udp::endpoint getEndpoint() const { return _endpoint; }
    [[nodiscard]] time_point<system_clock> getLastSeenTimestamp() const { return _lastSeen; }

private:
    const udp::endpoint _endpoint;
    SendToPeerFunction _sendToPeerFunction;

    // RUDP state
    uint32_t _localSequenceId = 0;
    uint32_t _remoteSequenceId = 0;

    time_point<system_clock> _lastSeen;
};

}
