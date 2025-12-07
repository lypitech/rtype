#pragma once

#include "Peer.hpp"
#include "Session.hpp"
#include "logger/Logger.h"

namespace rtnt::core
{

/**
 * @class   Client
 * @brief   Basically a client connected to a single host.
 *
 * The Client wraps a single Session object pointing to the remote server.
 * It filters incoming traffic to ensure we only process packets from the host we connected to.
 */
class Client : public Peer
{

using OnMessageFunction = std::function<void(Packet&)>;
using OnConnectFunction = std::function<void()>;
using OnDisconnectFunction = std::function<void()>;

public:
    explicit Client(asio::io_context& context)
        : Peer(context)
    {}

    void onDisconnect(OnDisconnectFunction callback) { _onDisconnect = std::move(callback); }
    void onMessage(OnMessageFunction callback) { _onMessage = std::move(callback); }

    /**
     * @brief   Initiates the connection handshake.
     *
     * This creates the Session and sends an initial 'CONNECT' packet to the server.
     * @param   ip      The server IP address.
     * @param   port    The server port.
     */
    void connect(
        const std::string& ip,
        unsigned short port
    );

    /**
     * @brief   Quick helper to send a user-defined Packet to the server.
     * @param   packet Packet to send to the server
     */
    void send(Packet packet) { LOG_DEBUG("Sending a packet from client."); _serverSession->send(packet); }

    /**
     * @brief   Main maintenance loop. Checks for timeouts.
     * @param   timeout The duration after which the server is considered unresponsive and so, dead
     * @note    This should be called regularly (e.g., in a main game loop).
     */
    void update(milliseconds timeout = seconds(10));

protected:
    void onReceive(
        const udp::endpoint& sender,
        ByteBuffer& data
    ) override;

private:
    udp::endpoint _serverEndpoint;
    std::shared_ptr<Session> _serverSession;
    // bool _isConnected = false;

    OnDisconnectFunction _onDisconnect;
    OnMessageFunction _onMessage;
};

}
