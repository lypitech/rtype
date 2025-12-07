#include "rtnt/Core/Peer.hpp"

#include "logger/Logger.h"

namespace rtnt::core
{

void Peer::sendToTarget(
    const udp::endpoint &target,
    const ByteBuffer &data
)
{
    _socket.async_send_to(
        asio::buffer(data),
        target,
        [target](std::error_code ec, size_t bytesSent) {
            if (ec) {
                LOG_WARN("Encountered an error while sending data: {}.", ec.message());
                return;
            }

            LOG_TRACE_R3(
                "Sent {} bytes to {}.",
                bytesSent,
                target.address().to_string()
            );
        }
    );
}

void Peer::receive()
{
    LOG_DEBUG("Listening...");

    _socket.async_receive_from(
        asio::buffer(_receptionBuffer),
        _tmpEndpoint,
        [this](std::error_code ec, size_t bytesReceived) {
            if (ec) {
                if (ec != asio::error::operation_aborted) { // This error is thrown when the socket is intentionally closed
                    LOG_WARN("Encountered an error while receiving data: {}.", ec.message());
                    receive();
                }
                return;
            }

            LOG_TRACE_R3(
                "Received {} bytes from {}.",
                bytesReceived,
                _tmpEndpoint.address().to_string()
            );

            if (bytesReceived > 0) {
                // todo: optimization is possible by making a buffer pool (avoiding buffer recreation c;)
                ByteBuffer data(
                    _receptionBuffer.begin(),
                    _receptionBuffer.begin() + bytesReceived
                );
                onReceive(_tmpEndpoint, data);
            }
            receive();
        }
    );
}

}
