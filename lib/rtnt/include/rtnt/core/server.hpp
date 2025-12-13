#pragma once

#include <map>
#include <ranges>
#include <utility>

#include "dispatcher.hpp"
#include "peer.hpp"
#include "session.hpp"

namespace rtnt::core {

/**
 * @class   Server
 * @brief   Manages multiple client sessions and routes traffic (TCAS haha lol traffic traffic)
 *
 * The Server listens on a specific port and maintains a map of active Sessions.
 * It routes incoming data from the Peer to the correct Session according to the sender's Endpoint.
 */
class Server : public Peer
{
    using OnConnectFunction = std::function<void(std::shared_ptr<Session>)>;
    using OnDisconnectFunction = std::function<void(std::shared_ptr<Session>)>;
    using OnMessageFunction = std::function<void(std::shared_ptr<Session>, Packet&)>;

public:
    explicit Server(asio::io_context& context, unsigned short port);

    /**
     * @brief   Sets the callback for when a remote Peer connects to the Server.
     * @note    A remote Peer is considered connected if he sends the packet @code __rtnt_internal_CONNECT@endcode and
     *          the server accepts it.
     * @note    We recommend providing a lambda function.
     * @param   callback    Function signature: @code void(std::shared_ptr<Session>)@endcode
     */
    void onConnect(OnConnectFunction callback) { _onConnect = std::move(callback); }

    /**
     * @brief   Sets the callback for when a known session disconnects from the Server (either by timing out or just
     * leaving on purpose).
     * @note    We recommend providing a lambda function.
     * @param   callback    Function signature: @code void(std::shared_ptr<Session>)@endcode
     */
    void onDisconnect(OnDisconnectFunction callback) { _onDisconnect = std::move(callback); }

    /**
     * @brief   Sets the callback for when a valid packet is received from a known session.
     * @note    We recommend providing a lambda function.
     * @param   callback    Function signature: @code void(std::shared_ptr<Session>, Packet&)@endcode
     */
    void onMessage(OnMessageFunction callback) { _onMessage = std::move(callback); }

    /**
     * @brief   Main maintenance loop. Checks for timeouts.
     * @param   timeout The duration after which a client is considered unresponsive and so, dead
     * @note    This should be called regularly (e.g., in a main game loop).
     */
    void update(milliseconds timeout = seconds(10));

    template <typename T>
    void sendTo(const std::shared_ptr<Session>& session, const T& packetData)
    {
        packet::verifyUserPacketData<T>();
        _internal_sendTo(session, packetData);
    }

    template <typename T>
    void broadcast(const T& packetData)
    {
        packet::verifyUserPacketData<T>();
        for (auto &session: _sessions | std::views::values) {
            _internal_sendTo(session, packetData);
        }
    }

    // todo: maybe a broadcast function to send a packet to everyone?

    [[nodiscard]] Dispatcher& getPacketDispatcher() { return this->_packetDispatcher; }

protected:
    void onReceive(const udp::endpoint& sender, std::shared_ptr<ByteBuffer> data) override;

private:
    std::map<udp::endpoint, std::shared_ptr<Session>> _sessions;

    Dispatcher _packetDispatcher;

    OnConnectFunction _onConnect;
    OnDisconnectFunction _onDisconnect;
    OnMessageFunction _onMessage;

    template <typename T>
    void _internal_sendTo(const std::shared_ptr<Session>& session, const T& packetData)
    {
        packet::verifyPacketData<T>();

        LOG_DEBUG("Server sending Packet #{} {}...", T::kId, packet::getName<T>());

        Packet packetToSend(T::kId, packet::getFlag<T>());
        packetToSend << packetData;
        session->send(packetToSend);
    }
};

}  // namespace rtnt::core
