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
    if (!_isConnected) {
        LOG_WARN("Trying to disconnect while not connected.");
        return;
    }

    constexpr packet::internal::Disconnect packet{};
    _internal_send(packet);
    // stop(); /// fixme: this makes the program crash because of Asio async operations (closing the hardware interface before finishing async work). Maybe move this at another place?
    _serverSession.reset();
    _isConnected = false;

    if (_onDisconnect) {
        _onDisconnect();
    }
}

void Client::update(milliseconds timeout)
{
    if (!_serverSession) {
        LOG_WARN("Trying to update without initiating connection first.");
        return;
    }

    auto now = steady_clock::now();

    if (!_isConnected) {
        // _serverSession->update();

        bool timedOut = (now - _lastConnectionAttemptTime) > RECONNECTION_TIMEOUT;
        bool sessionFailed = _serverSession->shouldClose();

        if (timedOut || sessionFailed) {
            if (_reconnectionRetries <= MAX_RECONNECTION_RETRIES) {
                _reconnectionRetries++;
                LOG_WARN("Connection attempt {}/{} timed out. Retrying...",
                         _reconnectionRetries,
                         MAX_RECONNECTION_RETRIES);

                _internal_attemptConnection();
            } else {
                LOG_FATAL("Could not connect to server after {} attempts. Aborting.",
                          MAX_RECONNECTION_RETRIES);
                _serverSession.reset();
                if (_onDisconnect) {
                    _onDisconnect();
                }
            }
        }
        return;
    }

    if (_serverSession->shouldClose()) {
        LOG_INFO("Disconnected by server");
        if (_onDisconnect) {
            _onDisconnect();
        }
        stop();  /// fixme: can make the program crash under certain circumstances.
        _serverSession.reset();
        _isConnected = false;
        return;
    }

    auto lastSeenTimestamp = _serverSession->getLastSeenTimestamp();
    auto age = duration_cast<milliseconds>(now - lastSeenTimestamp);

    if (age > timeout) {
        LOG_FATAL("Server timeout exceeded, disconnecting...");

        disconnect();
        // todo: maybe try a reconnect mechanism? like you try to reconnect 3 times and if it still fails, then abort
    }

    _serverSession->update();
}

void Client::onReceive(const udp::endpoint& sender,
                       std::shared_ptr<ByteBuffer> data)
{
    if (sender != _serverEndpoint || !_serverSession) {
        LOG_WARN(
            "Received data that doesn't come from the remote server. Probably random internet "
            "noise, skipping...");
        return;
    }

    auto packetsToProcess = _serverSession->handleIncoming(data);

    if (packetsToProcess.empty()) {
        return;
    }

    for (Packet& packet : packetsToProcess) {
        _packetDispatcher.dispatch(_serverSession, packet);

        if (_isConnected && _onMessage) {
            _onMessage(packet);
        }
    }
}

void Client::_internal_registerInternalPacketHandlers()
{
    _packetDispatcher._internal_bind<packet::internal::ConnectAck>(
        [this](const std::shared_ptr<Session>& /*session*/,
               const packet::internal::ConnectAck& packet) {
            LOG_DEBUG("Handshake success. Assigned Session ID: {}", packet.assignedSessionId);
            this->_isConnected = true;

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
    _internal_send(packet);
}

}  // namespace rtnt::core
