#pragma once

#include "Dispatcher.hpp"
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

using OnConnectFunction = std::function<void()>;
using OnDisconnectFunction = std::function<void()>;
using OnMessageFunction = std::function<void(Packet&)>;

public:
    explicit Client(asio::io_context& context);

    void onConnect(OnConnectFunction callback) { _onConnect = std::move(callback); }
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
     * @tparam  T           Type of user-defined packet (automatically resolved with variable)
     * @param   packetData  Packet to send
     */
    template <typename T>
    void send(const T& packetData)
    {
        packet::verifyUserPacketData<T>();
        internal_send<T>(packetData);
    }

    /**
     * @brief   Main maintenance loop. Checks for timeouts.
     * @param   timeout The duration after which the server is considered unresponsive and so, dead
     * @note    This should be called regularly (e.g., in a main game loop).
     */
    void update(milliseconds timeout = seconds(10));

    [[nodiscard]] bool isConnected() const { return _isConnected; }
    [[nodiscard]] Dispatcher& getPacketDispatcher() { return this->_packetDispatcher; }

protected:
    void onReceive(
        const udp::endpoint& sender,
        std::shared_ptr<ByteBuffer> data
    ) override;

private:
    udp::endpoint _serverEndpoint;
    std::shared_ptr<Session> _serverSession;
    bool _isConnected = false;

    Dispatcher _packetDispatcher;

    OnConnectFunction _onConnect;
    OnDisconnectFunction _onDisconnect;
    OnMessageFunction _onMessage;

    /**
     * @brief   Binds all internal packet handlers
     */
    void _internal_registerInternalPacketHandlers();

    /**
     * @brief   Sends a packet to the connected server.
     * @tparam  T           Type of packet to send (automatically resolved with variable)
     * @param   packetData  Packet to send
     */
    template <typename T>
    void _internal_send(const T& packetData)
    {
        packet::verifyPacketData<T>();

        LOG_DEBUG(
            "Client sending Packet #{} {}...",
            T::kId,
            packet::getName<T>()
        );

        Packet packetToSend(T::kId, packet::getFlag<T>());
        packetToSend << packetData;
        _serverSession->send(packetToSend);
    }

};

}
