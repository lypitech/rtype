#pragma once

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>

#include "Packet.hpp"

static constexpr size_t BUFFER_SIZE = USHRT_MAX;

namespace rtnt::core
{

using asio::ip::udp;

/**
 * @class   Peer
 * @brief   Abstract base class for any network entity (Client or Server).
 *
 * The Peer class is just an Asio wrapper that listens and speaks to targets
 * (can be clients or the server, depending on the selected mode).
 *
 * It handles raw I/O operations:
 * - Binding to a port
 * - Asynchronous receiving loop
 * - Sending raw byte buffers
 *
 * @warning This class does NOT parse packets. It sticks to raw bytes, both for
 * receiving and sending data.
 */
class Peer
{
public:
    virtual ~Peer() = default;

    /**
     * @brief   Server mode.
     *
     * @param   context Asio I/O context
     * @param   port    Port the server will listen to
     */
    explicit Peer(
        asio::io_context& context,
        unsigned short port
    )   : _context(context)
        , _socket(
            context,
            udp::endpoint(udp::v4(), port)
        )
    {}

    /**
     * @brief   Client mode.
     *
     * Port will be automatically chose by the OS.
     *
     * @param   context Asio I/O context
     */
    explicit Peer(asio::io_context& context)
        : _context(context)
        , _socket(
            context,
            udp::endpoint(udp::v4(), 0)
        )
    {}

    /**
     * @brief   Starts the asynchronous receive() loop.
     * @note    Requires the associated `io_context` to be running.
     */
    void start() { receive(); }

    /**
     * @brief   Sends raw bytes to a specific target.
     *
     * @param   target  Target to send the data to
     * @param   data    Data to send (raw bytes)
     * @note    This is a fire-and-forget operation. No delivery guarantee at this level (managed by RUDP, Session).
     */
    void sendToTarget(
        const udp::endpoint& target,
        const ByteBuffer& data
    );

    /**
     * @return  The local port the Peer is bound to.
     */
    [[nodiscard]] uint16_t getLocalPort() const { return _socket.local_endpoint().port(); }

protected:
    /**
     * @brief   Callback triggered when raw bytes are received.
     * @param   sender  The endpoint that sent the data
     * @param   data    The raw data received (raw bytes)
     */
    virtual void onReceive(
        const udp::endpoint& sender,
        ByteBuffer& data
    ) = 0;

private:
    asio::io_context& _context;
    udp::socket _socket;
    udp::endpoint _tmpEndpoint;
    std::array<char, BUFFER_SIZE> _receptionBuffer{};

    void receive();
};

}
