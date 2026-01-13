#include "rtnt/core/server.hpp"

#include "logger/Logger.h"
#include "rtnt/core/packets/connect.hpp"

namespace rtnt::core {

Server::Server(asio::io_context& context,
               const unsigned short port)
    : Peer(context)
{
    server(port);
}

void Server::update(milliseconds timeout)
{
    _processEvents();

    std::vector<std::shared_ptr<Session>> disconnectedSessions;

    {
        std::lock_guard lock(_sessionsMutex);

        const auto now = steady_clock::now();

        LOG_TRACE_R3("Updating server state. Time is {}", now.time_since_epoch().count());
        for (auto it = _sessions.begin(); it != _sessions.end();) {
            auto& session = it->second;
            auto lastSeen = session->getLastSeenTimestamp();
            auto age = duration_cast<milliseconds>(now - lastSeen);

            if (age > timeout || session->shouldClose()) {
                disconnectedSessions.push_back(session);
                it = _sessions.erase(it);
            } else {
                session->update();
                ++it;
            }
        }
    }

    for (const auto& session : disconnectedSessions) {
        if (_onDisconnect) {
            _onDisconnect(session);
        }
    }
}

void Server::onReceive(const udp::endpoint& sender,
                       std::shared_ptr<ByteBuffer> data)
{
    std::shared_ptr<Session> session;
    bool isNewConnection = false;

    {
        std::lock_guard lock(_sessionsMutex);

        auto it = _sessions.find(sender);

        if (it != _sessions.end()) {  // Session found
            session = it->second;

            if (packet::is<packet::internal::Connect>(*data)) {
                LOG_DEBUG("Received duplicate CONNECT from existing session. Resending ACK.");

                packet::internal::ConnectAck ackPacket;
                ackPacket.assignedSessionId = session->getId();

                Packet p(packet::internal::ConnectAck::kId, packet::internal::ConnectAck::kFlag);
                p << ackPacket;
                session->send(p);
                return;
            }
        } else {  // New connection
            if (!packet::is<packet::internal::Connect>(
                    *data)) {  // todo: you can optimize this because another call to Header::parse is made in Session::handleIncoming.
                LOG_DEBUG("Not CONNECT packet, ignoring...");
                return;
            }

            LOG_DEBUG("Is CONNECT packet, creating session.");

            session = std::make_shared<Session>(
                sender, [this, sender](std::shared_ptr<ByteBuffer> rawBytes) {
                    this->sendToTarget(sender, rawBytes);
                });
            _sessions[sender] = session;
            isNewConnection = true;
        }
    }

    if (isNewConnection) {
        _eventQueue.push([this, session]() {
            if (_onConnect) {
                _onConnect(session);
            }
        });
    }

    auto packetsToProcess = session->handleIncoming(data);

    if (packetsToProcess.empty()) {
        return;
    }

    for (Packet& packet : packetsToProcess) {
        _eventQueue.push([this, session, pkt = packet]() mutable {
            _packetDispatcher.dispatch(session, pkt);
            if (pkt.getId() >= 128 && _onMessage) {
                _onMessage(session, pkt);
            }
        });
    }
}

/// Used to process events on main thread and not on io thread
void Server::_processEvents()
{
    while (auto task = _eventQueue.pop()) {
        (*task)();
    }
}

}  // namespace rtnt::core
