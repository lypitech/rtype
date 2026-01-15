#pragma once

#include "dispatcher.hpp"
#include "logger/Logger.h"
#include "peer.hpp"
#include "rtnt/common/thread_safe_queue.hpp"
#include "session.hpp"

namespace rtnt::core {

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

    using Task = std::function<void()>;

public:
    explicit Client(asio::io_context& context);

    /**
     * @brief   Sets the callback for when client successfully connects with the remote server.
     * @note    The connection is considered successful whenever the @code __rtnt_internal_CONNECT_ACK@endcode packet
                is received.
     * @note    We recommend providing a lambda function.
     * @param   callback    Function signature: @code void(std::shared_ptr<Session>)@endcode
     */
    void onConnect(OnConnectFunction callback) { _onConnect = std::move(callback); }

    /**
     * @brief   Sets the callback for when the client disconnects.
     * @note    This function isn't only called when calling the @code disconnect()@endcode function. The remote server
     *          can disconnect the Client, in which case this the callback will also be triggered.
     * @note    We recommend providing a lambda function.
     * @param   callback    Function signature: @code void(std::shared_ptr<Session>)@endcode
     */
    void onDisconnect(OnDisconnectFunction callback) { _onDisconnect = std::move(callback); }

    /**
     * @brief   Sets the callback for when a valid packet is received from the remote server.
     * @note    We recommend providing a lambda function.
     * @param   callback    Function signature: @code void(std::shared_ptr<Session>, Packet&)@endcode
     */
    void onMessage(OnMessageFunction callback) { _onMessage = std::move(callback); }

    /**
     * @brief   Initiates the connection handshake.
     *
     * This creates the Session and sends an initial @code __rtnt_internal_CONNECT@endcode packet to
     * the server.
     * If the server doesn't respond to the handshake within @code RECONNECTION_TIMEOUT@endcode
     * milliseconds, a reconnection is tried. After failing @code MAX_RECONNECTION_ATTEMPTS@endcode
     * times, the client considers that the server can't be connected to.
     * @param   ip      The server IP address.
     * @param   port    The server port.
     */
    void connect(const std::string& ip,
                 unsigned short port);

    /**
     * @brief   Disconnects the client from the remote server
     *
     * Sends a @code __rtnt_internal_DISCONNECT@endcode to the server and immediately closes.
     */
    void disconnect();

    /**
     * @brief   Quick helper to send a user-defined Packet to the server.
     * @tparam  T           Type of user-defined packet (automatically resolved with variable)
     * @param   packetData  Packet to send
     */
    template <typename T>
    void send(const T& packetData)
    {
        packet::verifyUserPacketData<T>();
        _internal_send<T>(packetData);
    }

    /**
     * @brief   Main maintenance loop. Checks for timeouts and lost packets.
     * @param   timeout The duration after which the server is considered unresponsive and so, dead
     * @note    This should be called regularly (e.g., in a main game loop).
     */
    void update(milliseconds timeout = seconds(10));

    [[nodiscard]] bool isConnected() const
    {
        std::lock_guard lock(_mutex);
        return _isConnected;
    }

    [[nodiscard]] Dispatcher& getPacketDispatcher() { return this->_packetDispatcher; }

protected:
    void onReceive(const udp::endpoint& sender,
                   std::shared_ptr<ByteBuffer> data) override;

private:
    udp::endpoint _serverEndpoint;
    std::shared_ptr<Session> _serverSession;
    mutable std::mutex _mutex;

    ThreadSafeQueue<Task> _eventQueue;

    // Connection state //
    bool _isConnected = false;
    uint8_t _reconnectionRetries = 0;
    time_point<steady_clock> _lastConnectionAttemptTime;
    // ---------------- //

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

        std::shared_ptr<Session> session;

        {
            std::lock_guard lock(_mutex);
            session = _serverSession;
        }

        if (session) {
            LOG_DEBUG("Client sending Packet #{} {}...", T::kId, packet::getName<T>());

            Packet packetToSend(T::kId, packet::getFlag<T>(), packet::getChannelId<T>());
            packetToSend << packetData;
            _serverSession->send(packetToSend);
        }
    }

    /**
     * @brief   Attempts a connection to the remote server.
     *
     * This function resets the server session (any old pending packets will be erased) and sends a
     * @code __rtnt_internal_CONNECT@endcode packet to the remote server.
     */
    void _internal_attemptConnection();

    /**
     * @brief   Processes the events that have been received so far.
     * @note    This function MUST be called from the main thread. Not doing so would result in
     *          thread issues (data races).
     */
    void _processEvents();
};

}  // namespace rtnt::core
