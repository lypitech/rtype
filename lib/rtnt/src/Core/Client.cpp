#include "rtnt/Core/Client.hpp"
#include "rtnt/Core/Packets/Connect.hpp"

#include "logger/Logger.h"

namespace rtnt::core
{

void Client::connect(
    const std::string &ip,
    unsigned short port
)
{
    if (!_onDisconnect || !_onMessage) {
        LOG_CRIT("Make sure you set the callback functions before trying to connect.");
        return;
    }

    LOG_INFO("Connecting to server {}:{}...", ip, port);

    asio::ip::address address = asio::ip::make_address(ip);

    _serverEndpoint = udp::endpoint(address, port);
    _serverSession = std::make_shared<Session>(
        _serverEndpoint,
        [this](const ByteBuffer& rawBytes) {
            this->sendToTarget(_serverEndpoint, rawBytes);
        }
    );
    start();

    packet::internal::Connect packet;
    sendInternal(packet);
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
    const udp::endpoint &sender,
    ByteBuffer &data
)
{
    if (sender != _serverEndpoint) {
        return;
    }

    if (!_serverSession) {
        return;
    }

    Packet packet(0);

    if (_serverSession->handleIncoming(data, packet)) {
        _packetDispatcher.dispatch(_serverSession, packet);
        if (_onMessage) {
            _onMessage(packet);
        }
    }
}

}
