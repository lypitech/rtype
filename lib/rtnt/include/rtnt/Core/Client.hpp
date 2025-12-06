#pragma once

#include "Peer.hpp"
#include "Session.hpp"

namespace rtnt::core
{

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

    void connect(const std::string& ip, unsigned short port);

    void send(Packet packet) { _serverSession->send(packet); }

    void update(milliseconds timeout = seconds(10));

protected:
    void onReceive(
        const udp::endpoint& sender,
        ByteBuffer& data
    ) override;

private:
    udp::endpoint _serverEndpoint;
    std::shared_ptr<Session> _serverSession;
    bool _isConnected = false;

    OnDisconnectFunction _onDisconnect;
    OnMessageFunction _onMessage;
};

}
