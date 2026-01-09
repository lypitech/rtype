#include "rtnt/core/session.hpp"

#include <ranges>

#include "logger/Logger.h"
#include "rtnt/common/constants.hpp"

namespace rtnt::core {

static std::atomic<session::Id> globalSessionIdCounter{0};

Session::Session(udp::endpoint endpoint,
                 SendToPeerFunction sendToPeerFunction)
    : _id(globalSessionIdCounter++),
      _endpoint(std::move(endpoint)),
      _sendToPeerFunction(std::move(sendToPeerFunction)),
      _lastSeen(steady_clock::now())
{
}

std::vector<Packet> Session::handleIncoming(std::shared_ptr<ByteBuffer> rawData)
{
    std::lock_guard lock(_mutex);

    std::vector<Packet> readyPackets;

    LOG_TRACE_R3(
        "Handling incoming raw data\n"
        "Size: {} bytes\n"
        "Data (N): {}",
        rawData->size(),
        byteBufferToHexString(*rawData));

    const packet::parsing::Result headerParsingResult = packet::Header::parse(*rawData);

    if (!headerParsingResult) {
        LOG_ERR("Error while handling packet: {}",
                packet::parsing::to_string(headerParsingResult.error));
        return readyPackets;
    }

    const packet::Header& header = *headerParsingResult.header;

    _lastSeen = steady_clock::now();

    if (!_sentPackets.empty()) {
        LOG_DEBUG("Checking sent packets buffer...");
        _sentPackets.erase(header.acknowledgeId);
        for (int i = 0; i < 32; ++i) {
            if (header.acknowledgeBitfield & (1 << i)) {
                LOG_DEBUG(
                    "Packet (seqID: {}) acknowledged, removing...", header.acknowledgeId - (i + 1));
                _sentPackets.erase(header.acknowledgeId - (i + 1));
            }
        }
    }

    updateAcknowledgeInfo(header.sequenceId);

    size_t payloadSize = rawData->size() - sizeof(packet::Header);

    if (payloadSize == 0 &&
        header.messageId == static_cast<packet::Id>(packet::SystemMessageId::kAck)) {
        LOG_TRACE_R3("Received ACK packet, stopping.");
        return readyPackets;
    }

    Packet incomingPacket(header.messageId, static_cast<packet::Flag>(header.flags));
    ByteBuffer payload;
    payload.assign(rawData->begin() + sizeof(packet::Header), rawData->end());
    incomingPacket._internal_setPayload(std::move(payload));

    bool isOrdered =
        (incomingPacket.getReliability() & packet::Flag::kOrdered) == packet::Flag::kOrdered;

    LOG_DEBUG("Is packet ordered? {}.", isOrdered ? "Yes" : "No");

    if (!isOrdered) {
        readyPackets.push_back(std::move(incomingPacket));
        return readyPackets;
    }

    uint32_t receivedOrderId = header.orderId;

    LOG_DEBUG("Received ordered ID: {}", receivedOrderId);

    if (receivedOrderId == _nextExpectedOrderId) {
        readyPackets.push_back(std::move(incomingPacket));
        _nextExpectedOrderId++;

        while (_reorderBuffer.contains(_nextExpectedOrderId)) {
            auto node = _reorderBuffer.extract(_nextExpectedOrderId);
            readyPackets.push_back(std::move(node.mapped()));
            _nextExpectedOrderId++;
        }
    } else if (receivedOrderId > _nextExpectedOrderId) {
        LOG_TRACE_R2(
            "Gap: Got order ID {}, expected {}. Buffering.", receivedOrderId, _nextExpectedOrderId);
        _reorderBuffer[receivedOrderId] = std::move(incomingPacket);
    }

    return readyPackets;
}

void Session::send(Packet& packet)
{
    std::lock_guard lock(_mutex);

    uint32_t sequenceId = _localSequenceId++;
    uint32_t orderId = 0;

    if ((packet.getReliability() & packet::Flag::kOrdered) == packet::Flag::kOrdered) {
        orderId = _localOrderId++;
    }

    if (packet.getReliability() != packet::Flag::kUnreliable) {
        _sentPackets[sequenceId] = SentPacketInfo{packet, steady_clock::now(), sequenceId, orderId};
    }

    rawSend(packet, sequenceId, orderId);
}

void Session::rawSend(Packet& packet,
                      uint32_t sequenceId,
                      uint32_t orderId)
{
    packet::Header header{};

    header.sequenceId = sequenceId;
    header.orderId = orderId;
    header.acknowledgeId = _remoteAcknowledgeId;
    header.acknowledgeBitfield = _remoteAcknowledgeBitfield;
    header.messageId = packet.getId();
    header.flags = static_cast<uint8_t>(packet.getReliability());
    header.packetSize = static_cast<uint16_t>(packet.getPayload().size());
    header.checksum = 0;  // todo: Implement CRC32 checksum

    const auto rawBuffer = std::make_shared<ByteBuffer>();
    const auto& payload = packet.getPayload();

    rawBuffer->reserve(sizeof(packet::Header) + payload.size());

    header.convertEndianness();
    const auto* headerPtr = reinterpret_cast<const uint8_t*>(&header);
    rawBuffer->insert(rawBuffer->end(), headerPtr, headerPtr + sizeof(packet::Header));
    rawBuffer->insert(rawBuffer->end(), payload.begin(), payload.end());

    header.convertEndianness();
    LOG_TRACE_R3(
        "Preparing to send a packet.\n"
        "Sequence ID: {}\n"
        "Order ID: {}\n"
        "Acknowledge ID: {}\n"
        "Acknowledge bitfield: {}\n"
        "Message ID: {}\n"
        "Flags: {}\n"
        "Payload Size: {}\n"
        "Checksum: {}\n"
        "Raw header (H): {}\n"
        "Raw buffer (N): {}",
        header.sequenceId,
        header.orderId,
        header.acknowledgeId,
        header.acknowledgeBitfield,
        header.messageId,
        header.flags,
        header.packetSize,
        header.checksum,
        byteBufferToHexString(rawBuffer->begin(), rawBuffer->begin() + sizeof(packet::Header)),
        byteBufferToHexString(rawBuffer->begin() + sizeof(packet::Header), rawBuffer->end()));

    if (_sendToPeerFunction) {
        _sendToPeerFunction(rawBuffer);
    }

    _hasUnsentAck = false;
    _lastAckTime = steady_clock::now();
}

void Session::update()
{
    std::lock_guard lock(_mutex);

    LOG_DEBUG("Updating session {}", _id);

    auto now = steady_clock::now();

    LOG_DEBUG("Buffer state: size {}", _sentPackets.size());

    for (auto it = _sentPackets.begin(); it != _sentPackets.end();) {
        SentPacketInfo& info = it->second;

        LOG_DEBUG("Iterating over packet ordID = {}", info.orderId);
        if (now - info.sentTime > packet::RESEND_TIMEOUT) {
            if (info.retries >= packet::MAX_RESEND_RETRIES) {
                LOG_FATAL("Connection lost (Packet #{} retries exceeded).", it->first);
                disconnect();
                return;
            }

            LOG_TRACE_R2("Resending packet #{} ({}th retry, sequence ID = {} ; order ID = {})",
                         info.packet._messageId,
                         info.retries,
                         info.sequenceId,
                         info.orderId);
            rawSend(info.packet, info.sequenceId, info.orderId);

            info.sentTime = now;
            info.retries++;
        }
        ++it;
    }

    if (_hasUnsentAck && (now - _lastAckTime > packet::ACK_TIMEOUT)) {
        _internal_sendAck();
    }
}

void Session::disconnect()
{
    std::lock_guard lock(_mutex);
    this->_shouldClose = true;
}

// void Session::sendAck()
// {
//     LOG_TRACE_R3("Sending empty ACK packet");
//     Packet ack(
//         static_cast<packet::Id>(packet::SystemMessageId::kAck), packet::Flag::kUnreliable);
//     send(ack);
// }

void Session::_internal_sendAck()
{
    Packet ack(static_cast<packet::Id>(packet::SystemMessageId::kAck), packet::Flag::kUnreliable);
    uint32_t sequenceId = _localSequenceId++;

    rawSend(ack, sequenceId, 0);
}

void Session::updateAcknowledgeInfo(uint32_t sequenceId)
{
    LOG_DEBUG("Updating acknowledge information");

    if (sequenceId > _remoteAcknowledgeId) {
        uint32_t shift = sequenceId - _remoteAcknowledgeId;

        if (shift > 32) {
            _remoteAcknowledgeBitfield = 0;
        } else {
            _remoteAcknowledgeBitfield <<= shift;
            _remoteAcknowledgeBitfield |= 1 << (shift - 1);
        }
        _remoteAcknowledgeId = sequenceId;
    } else if (sequenceId < _remoteAcknowledgeId) {
        uint32_t diff = _remoteAcknowledgeId - sequenceId;

        if (diff > 0 && diff <= 32) {
            _remoteAcknowledgeBitfield |= 1 << (diff - 1);
        }
    }

    _hasUnsentAck = true;
}

}  // namespace rtnt::core
