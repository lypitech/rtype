#include "rtnt/Core/Server.hpp"

#include "logger/Logger.h"
#include "rtnt/Core/Packets/Connect.hpp"

namespace rtnt::core {

void Server::update(milliseconds timeout)
{
    auto now = steady_clock::now();

    for (auto it = _sessions.begin(); it != _sessions.end();) {
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

void Server::onReceive(const udp::endpoint& sender, std::shared_ptr<ByteBuffer> data)
{
    std::shared_ptr<Session> session;

    auto it = _sessions.find(sender);

    if (it != _sessions.end()) {  // Session found
        session = it->second;
    } else {  // New connection
        if (!packet::is<packet::internal::Connect>(
                *data)) {  // todo: you can optimize this because another call to Header::parse is made in Session::handleIncoming.
            LOG_TRACE_R3("Not CONNECT packet, ignoring...");
            return;
        }

        LOG_TRACE_R3("Is CONNECT packet, creating session.");

        session = std::make_shared<Session>(
            sender, [this, sender](std::shared_ptr<ByteBuffer> rawBytes) { this->sendToTarget(sender, rawBytes); });
        _sessions[sender] = session;

        if (_onConnect) {
            _onConnect(session);
        }
    }

    Packet packet(0);

    if (session->handleIncoming(data, packet)) {
        _packetDispatcher.dispatch(session, packet);
        if (_onMessage) {
            _onMessage(session, packet);
        }
    }
}

}  // namespace rtnt::core
