#include "rtnt/Core/Client.hpp"
#include "rtnt/Core/Packets/Connect.hpp"

#include "logger/Logger.h"

namespace rtnt::core
{

Client::Client(asio::io_context &context)
    : Peer(context)
{
    _internal_registerInternalPacketHandlers();
}

void Client::connect(
    const std::string &ip,
    unsigned short port
)
{
    if (!_onConnect || !_onDisconnect || !_onMessage) {
        LOG_CRIT("Make sure you set the callback functions before trying to connect.");
        return;
    }

    LOG_INFO("Connecting to server {}:{}...", ip, port);

    const asio::ip::address address = asio::ip::make_address(ip);

    _serverEndpoint = udp::endpoint(address, port);
    _serverSession = std::make_shared<Session>(
        _serverEndpoint,
        [this](std::shared_ptr<ByteBuffer> rawBytes) {
            this->sendToTarget(_serverEndpoint, rawBytes);
        }
    );
    start();

    packet::internal::Connect packet;
    _internal_send(packet);
}

void Client::update(milliseconds timeout)
{
    if (!_serverSession) {
        LOG_WARN("Trying to update while disconnected from server.");
        return;
    }

    auto now = steady_clock::now();
    auto lastSeenTimestamp = _serverSession->getLastSeenTimestamp();
    auto age = duration_cast<milliseconds>(now - lastSeenTimestamp);

    if (age > timeout) {
        LOG_FATAL("Server timeout exceeded, disconnecting...");

        if (_onDisconnect) {
            _onDisconnect();
        }
        // todo: maybe try a reconnect mechanism? like you try to reconnect 3 times and if it still fails, then abort
    }
}

void Client::onReceive(
    const udp::endpoint& sender,
    std::shared_ptr<ByteBuffer> data
)
{
    if (sender != _serverEndpoint || !_serverSession) {
        return;
    }

    Packet packet(0);

    if (_serverSession->handleIncoming(data, packet)) {
        _packetDispatcher.dispatch(_serverSession, packet);

        if (_isConnected && _onMessage) {
            _onMessage(packet);
        }
    }
}

void Client::_internal_registerInternalPacketHandlers()
{
    _packetDispatcher._internal_bind<packet::internal::ConnectAck>(
        [this](const std::shared_ptr<Session>& /*session*/, const packet::internal::ConnectAck& packet) {
            LOG_DEBUG("Received ID: {}", packet.assignedSessionId);
            this->_isConnected = true;

            if (_onConnect) {
                _onConnect();
            }
        }
    );
}

}
