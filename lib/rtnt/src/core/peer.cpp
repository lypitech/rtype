#include "rtnt/core/peer.hpp"

#include "logger/Logger.h"

namespace rtnt::core {

void Peer::server(const unsigned short port) { _socket = udp::socket(_context, udp::endpoint(udp::v4(), port)); }

void Peer::client() { _socket = udp::socket(_context, udp::endpoint(udp::v4(), 0)); }

void Peer::stop()
{
    _socket.shutdown(udp::socket::shutdown_send);
    _socket.close();
}

void Peer::receive()
{
    if (!_socket.is_open()) {
        LOG_ERR("Trying to receive with a closed socket.");
        return;
    }

    LOG_DEBUG("Listening...");

    _socket.async_receive_from(
        asio::buffer(_receptionBuffer), _tmpEndpoint, [this](std::error_code ec, size_t bytesReceived) {
            if (ec) {
                if (ec !=
                    asio::error::operation_aborted) {  // This error is thrown when the socket is intentionally closed
                    LOG_WARN("Encountered an error while receiving data: {}.", ec.message());
                    receive();
                }
                return;
            }

            LOG_TRACE_R3("Received {} bytes from {}:{}.", bytesReceived, _tmpEndpoint.address().to_string(),
                         _tmpEndpoint.port());

            if (bytesReceived > 0) {
                // todo: optimization is possible by making a buffer pool (avoiding buffer recreation c;)
                auto data =
                    std::make_shared<ByteBuffer>(_receptionBuffer.begin(), _receptionBuffer.begin() + bytesReceived);

                onReceive(_tmpEndpoint, data);
            }
            receive();
        });
}

void Peer::sendToTarget(const udp::endpoint &target, std::shared_ptr<ByteBuffer> data)
{
    _socket.async_send_to(asio::buffer(*data), target, [target, data](std::error_code ec, size_t bytesSent) {
        if (ec) {
            LOG_WARN("Encountered an error while sending data: {}.", ec.message());
            return;
        }

        LOG_TRACE_R3("Sent {} bytes to {}:{}.", bytesSent, target.address().to_string(), target.port());
    });
}

}  // namespace rtnt::core
