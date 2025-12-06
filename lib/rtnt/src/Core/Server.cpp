#include "rtnt/Core/Server.hpp"

#include "logger/Logger.h"

namespace rtnt::core
{

void Server::update(milliseconds timeout)
{
    auto now = system_clock::now();

    for (auto it = _sessions.begin(); it != _sessions.end(); ) {
        auto& session = it->second;
        auto lastSeen = session->getLastSeenTimestamp();
        auto age = duration_cast<milliseconds>(now - lastSeen);

        if (age > timeout) {
            if (_onDisconnect) {
                _onDisconnect(session);
            }
            it = _sessions.erase(it);
        } else {
            ++it;
        }
    }
}

void Server::onReceive(
    const udp::endpoint& sender,
    ByteBuffer& data
)
{
    std::shared_ptr<Session> session;

    auto it = _sessions.find(sender);

    if (it != _sessions.end()) { // Session found
        session = it->second;
    } else { // New connection
        // todo: only create a session if packet received is CONNECT.
        session = std::make_shared<Session>(
            sender,
            [this, sender](const ByteBuffer& rawBytes) {
                this->sendToTarget(sender, rawBytes);
            }
        );
        _sessions[sender] = session;

        if (_onConnect) {
            _onConnect(session);
        }
    }

    Packet packet(0);

    if (session->handleIncoming(data, packet)) {
        if (_onMessage) {
            _onMessage(session, packet);
        }
    }
}

}
