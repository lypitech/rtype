#pragma once

#include <map>
#include <utility>

#include "Peer.hpp"
#include "Session.hpp"

namespace rtnt::core
{

class Server : public Peer
{

using OnConnectFunction = std::function<void(std::shared_ptr<Session>)>;
using OnDisconnectFunction = std::function<void(std::shared_ptr<Session>)>;
using OnMessageFunction = std::function<void(std::shared_ptr<Session>, Packet&)>;

public:
    explicit Server(
        asio::io_context& context,
        unsigned short port
    )   : Peer(context, port)
    {}

    void onConnect(OnConnectFunction callback) { _onConnect = std::move(callback); }
    void onDisconnect(OnDisconnectFunction callback) { _onDisconnect = std::move(callback); }
    void onMessage(OnMessageFunction callback) { _onMessage = std::move(callback); }

    /**
     * @brief   Maintenance function. MUST be called regularly (e.g., once per frame/second).
     *
     * It checks for dead clients and removes them.
     */
    void update(milliseconds timeout = seconds(10));

    // todo: maybe a broadcast function to send a packet to everyone?

protected:
    void onReceive(
        const udp::endpoint& sender,
        ByteBuffer& data
    ) override;

private:
    std::map<udp::endpoint, std::shared_ptr<Session>> _sessions;

    OnConnectFunction _onConnect;
    OnDisconnectFunction _onDisconnect;
    OnMessageFunction _onMessage;
};

}
