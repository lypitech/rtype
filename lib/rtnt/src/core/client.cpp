#include "rtnt/core/client.hpp"

#include "logger/Logger.h"
#include "rtnt/core/packets/connect.hpp"
#include "rtnt/core/packets/disconnect.hpp"

namespace rtnt::core {

Client::Client(asio::io_context& context)
    : Peer(context)
{
    client();
    _internal_registerInternalPacketHandlers();
}

void Client::connect(const std::string& ip,
                     const unsigned short port)
{
    std::lock_guard lock(_mutex);

    if (_isConnected) {
        LOG_ERR("Trying to connect while already connected. Ignoring...");
        return;
    }

    if (!_onConnect) {
        LOG_WARN("onConnect callback has not been set.");
    }

    if (!_onDisconnect) {
        LOG_WARN("onDisconnect callback has not been set.");
    }

    if (!_onMessage) {
        LOG_WARN("onMessage callback has not been set.");
    }

    LOG_INFO("Connecting to remote server {}:{}...", ip, port);

    const asio::ip::address address = asio::ip::make_address(ip);
    _serverEndpoint = udp::endpoint(address, port);
    _reconnectionRetries = 0;

    start();
    _internal_attemptConnection();
}

void Client::disconnect()
{
    std::lock_guard lock(_mutex);

    if (_isConnected && _serverSession) {
        packet::internal::Disconnect packet{};
        _serverSession->send(packet);
    }

    _serverSession.reset();
    _isConnected = false;

    _eventQueue.push([this]() {
        if (_onDisconnect) {
            _onDisconnect();
        }
    });

    // stop(); /// fixme: this makes the program crash because of Asio async operations (closing the hardware interface before finishing async work). Maybe move this at another place?
}

void Client::update(milliseconds timeout)
{
    _processEvents();

    std::shared_ptr<Session> session;
    bool isConnected = false;

    {
        std::lock_guard lock(_mutex);
        session = _serverSession;
        isConnected = _isConnected;

        if (session && !isConnected) {
            auto now = steady_clock::now();
            bool timedOut = (now - _lastConnectionAttemptTime) > RECONNECTION_TIMEOUT;
            bool sessionFailed = session->shouldClose();

            if (timedOut || sessionFailed) {
                if (_reconnectionRetries < MAX_RECONNECTION_ATTEMPTS) {
                    _reconnectionRetries++;
                    LOG_WARN("Connection attempt {}/{} timed out ({} delay). Retrying...",
                             _reconnectionRetries,
                             MAX_RECONNECTION_ATTEMPTS,
                             RECONNECTION_TIMEOUT);
                    _internal_attemptConnection();
                    return;
                }
                LOG_FATAL("Could not connect to server after {} attempts. Aborting.",
                          MAX_RECONNECTION_ATTEMPTS);
                _serverSession.reset();
            }
        }
    }

    if (!session) {
        return;
    }

    if (session->shouldClose()) {
        LOG_INFO("Disconnected by server (session closed)");
        disconnect();
        return;
    }

    if (isConnected) {
        auto now = steady_clock::now();
        auto lastSeenTimestamp = session->getLastSeenTimestamp();
        auto age = duration_cast<milliseconds>(now - lastSeenTimestamp);

        if (age > timeout) {
            LOG_FATAL("Server timeout exceeded ({}ms), disconnecting...", age.count());
            disconnect();
            return;
        }
    }

    session->update();
}

void Client::onReceive(const udp::endpoint& sender,
                       std::shared_ptr<ByteBuffer> data)
{
    std::shared_ptr<Session> session;

    {
        std::lock_guard lock(_mutex);
        session = _serverSession;
    }

    if (sender != _serverEndpoint || !session) {
        LOG_WARN(
            "Received data that doesn't come from the remote server. Probably random internet "
            "noise, skipping...");
        return;
    }

    auto packetsToProcess = session->handleIncoming(data);

    if (packetsToProcess.empty()) {
        return;
    }

    for (Packet& packet : packetsToProcess) {
        _eventQueue.push([this, session, pkt = packet]() mutable {
            _packetDispatcher.dispatch(session, pkt);

            bool isConnected;
            {
                std::lock_guard lock(_mutex);
                isConnected = _isConnected;
            }

            if (isConnected && _onMessage) {
                _onMessage(pkt);
            }
        });
    }
}

void Client::_internal_registerInternalPacketHandlers()
{
    _packetDispatcher._internal_bind<packet::internal::ConnectAck>(
        [this](const std::shared_ptr<Session>& /*session*/,
               const packet::internal::ConnectAck& packet) {
            {
                std::lock_guard lock(_mutex);

                if (_isConnected) {
                    return;
                }

                LOG_DEBUG("Handshake success. Assigned Session ID: {}", packet.assignedSessionId);
                LOG_INFO("Successfully connected to server.");
                this->_isConnected = true;
            }

            if (_onConnect) {
                _onConnect();
            }
        });
}

void Client::_internal_attemptConnection()
{
    _serverSession =
        std::make_shared<Session>(_serverEndpoint, [this](std::shared_ptr<ByteBuffer> rawBytes) {
            this->sendToTarget(_serverEndpoint, rawBytes);
        });

    _lastConnectionAttemptTime = steady_clock::now();

    constexpr packet::internal::Connect packet;
    _serverSession->send(packet);
}

/// Used to process events on main thread and not on io thread
void Client::_processEvents()
{
    while (auto task = _eventQueue.pop()) {
        (*task)();
    }
}

}  // namespace rtnt::core
