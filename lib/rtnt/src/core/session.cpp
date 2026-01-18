#include "rtnt/core/session.hpp"

#include <ranges>

#include "logger/Logger.h"
#include "rtnt/common/constants.hpp"
#include "rtnt/core/packets/rich_ack.hpp"

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

    {
        auto& channelMetrics = _sessionMetrics.channels[header.channelId];
        channelMetrics.packetsReceived.fetch_add(1, std::memory_order_relaxed);
        channelMetrics.bytesReceived.fetch_add(rawData->size(), std::memory_order_relaxed);
    }

    bool hasAck = (header.flags & static_cast<uint8_t>(packet::Flag::kHasAck)) != 0;

    if (hasAck && !_sentPackets.empty()) {
        LOG_DEBUG("Checking sent packets buffer...");

        auto it = _sentPackets.find(header.acknowledgeId);

        if (it != _sentPackets.end()) {
            if (it->second.retries == 0) {
                auto now = steady_clock::now();
                auto rtt = duration_cast<milliseconds>(now - it->second.sentTime);

                _updateRtt(rtt);
            }
            _sentPackets.erase(it);
        }

        for (int i = 0; i < 32; ++i) {
            if (header.acknowledgeBitfield & (1 << i)) {
                LOG_DEBUG(
                    "Packet (seqID: {}) acknowledged, removing...", header.acknowledgeId - (i + 1));
                _sentPackets.erase(header.acknowledgeId - (i + 1));
            }
        }
    }

    bool isDuplicate = this->isDuplicate(header.sequenceId);

    if (isDuplicate) {
        LOG_WARN("Dropped duplicate packet #{}", header.sequenceId);
        ++_sessionMetrics.duplicateCount;
        _pendingRichAcks.push_back(header.sequenceId);
        _hasUnsentAck = true;
        return readyPackets;
    }

    updateAcknowledgeInfo(header.sequenceId);

    _packetsSinceLastAck++;

    // if we received enough packets to fill half our window, directly send ACK
    if (_packetsSinceLastAck >= packet::ACK_PACKET_THRESHOLD) {
        _internal_sendAck();
    }

    if (header.messageId == static_cast<packet::Id>(packet::SystemMessageId::kRichAck)) {
        LOG_TRACE_R3("Received RICH_ACK packet");

        checkForOldPackets(rawData, header);
        return readyPackets;
    }

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

    packet::ChannelId receivedChannelId = header.channelId;
    packet::OrderId receivedOrderId = header.orderId;

    LOG_DEBUG("Received channel ID: {}", receivedChannelId);

    packet::OrderId& nextExpected = _nextExpectedOrderIds[receivedChannelId];
    auto& reorderBuffer = _reorderBuffers[receivedChannelId];

    LOG_DEBUG("Received ordered ID: {}", receivedOrderId);

    if (receivedOrderId == nextExpected) {
        readyPackets.push_back(std::move(incomingPacket));
        nextExpected++;

        while (reorderBuffer.contains(nextExpected)) {
            auto node = reorderBuffer.extract(nextExpected);
            readyPackets.push_back(std::move(node.mapped()));
            nextExpected++;
        }
    } else if (receivedOrderId > nextExpected) {
        LOG_TRACE_R2(
            "Gap: Got order ID {}, expected {}. Buffering.", receivedOrderId, nextExpected);
        reorderBuffer[receivedOrderId] = std::move(incomingPacket);
    } else {
        LOG_WARN("Channel {}: Duplicate/Old Ordered Packet (Got {}, Expected {}). Ignoring.",
                 receivedChannelId,
                 receivedOrderId,
                 nextExpected);
    }

    return readyPackets;
}

void Session::send(Packet& packet)
{
    std::lock_guard lock(_mutex);

    packet::SequenceId sequenceId = _localSequenceId++;
    packet::OrderId orderId = 0;

    if ((packet.getReliability() & packet::Flag::kOrdered) == packet::Flag::kOrdered) {
        packet::ChannelId channel = packet.getChannel();
        orderId = _localOrderIds[channel]++;
    }

    if (packet.getReliability() != packet::Flag::kUnreliable) {
        _sentPackets[sequenceId] = SentPacketInfo{packet, steady_clock::now(), sequenceId, orderId};
    }

    rawSend(packet, sequenceId, orderId);
}

void Session::rawSend(Packet& packet,
                      packet::SequenceId sequenceId,
                      packet::OrderId orderId)
{
    packet::Header header{};

    header.sequenceId = sequenceId;
    header.channelId = packet.getChannel();
    header.orderId = orderId;
    header.acknowledgeId = _remoteAcknowledgeId;
    header.acknowledgeBitfield = _remoteAcknowledgeBitfield;
    header.messageId = packet.getId();
    header.flags = static_cast<uint8_t>(packet.getReliability());
    header.packetSize = static_cast<uint16_t>(packet.getPayload().size());
    // header.checksum = 0;  // todo: Implement CRC32 checksum

    if (_hasReceivedRemotePacket) {
        header.flags |= static_cast<uint8_t>(packet::Flag::kHasAck);
        _packetsSinceLastAck = 0;
    }

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
        // "Checksum: {}\n"
        "Raw header (H): {}\n"
        "Raw buffer (N): {}",
        header.sequenceId,
        header.orderId,
        header.acknowledgeId,
        bitfieldToString(header.acknowledgeBitfield),
        header.messageId,
        header.flags,
        header.packetSize,
        // header.checksum,
        byteBufferToHexString(rawBuffer->begin(), rawBuffer->begin() + sizeof(packet::Header)),
        byteBufferToHexString(rawBuffer->begin() + sizeof(packet::Header), rawBuffer->end()));

    if (_sendToPeerFunction) {
        _sendToPeerFunction(rawBuffer);
    }

    _hasUnsentAck = false;
    _lastAckTime = steady_clock::now();

    {
        auto& channelMetrics = _sessionMetrics.channels[packet.getChannel()];
        channelMetrics.packetsSent.fetch_add(1, std::memory_order_relaxed);
        channelMetrics.bytesSent.fetch_add(
            packet.getPayload().size() + sizeof(packet::Header), std::memory_order_relaxed);
    }
}

void Session::update()
{
    std::lock_guard lock(_mutex);

    LOG_DEBUG("Updating session {}", _id);

    auto now = steady_clock::now();

    for (auto it = _sentPackets.begin(); it != _sentPackets.end();) {
        SentPacketInfo& info = it->second;

        LOG_DEBUG("Iterating over packet ordID = {}", info.orderId);
        if (now - info.sentTime > packet::RESEND_TIMEOUT) {
            if (info.retries >= packet::MAX_RESEND_ATTEMPTS) {
                LOG_FATAL("Connection lost (Packet #{} retries exceeded).", info.packet._messageId);
                _internal_disconnect();
                return;
            }

            LOG_TRACE_R2("Resending packet #{} ({}/{} retry, sequence ID = {} ; order ID = {})",
                         info.packet._messageId,
                         info.retries,
                         packet::MAX_RESEND_ATTEMPTS,
                         info.sequenceId,
                         info.orderId);
            rawSend(info.packet, info.sequenceId, info.orderId);

            info.sentTime = now;
            info.retries++;
            ++_sessionMetrics.retransmitCount;
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
    _internal_disconnect();
}

void Session::_internal_sendAck()
{
    if (!_pendingRichAcks.empty()) {
        size_t totalAcks = _pendingRichAcks.size();
        size_t processed = 0;
        size_t chunkN = 1;

        LOG_TRACE_R2("Flushing {} pending ACKs in chunks", totalAcks);

        while (processed < totalAcks) {
            LOG_TRACE_R2("Chunk {}, processed = {}", chunkN++, processed);

            size_t chunkSize = std::min(packet::MAX_ACK_PER_PACKET, totalAcks - processed);

            auto start = _pendingRichAcks.begin() + processed;
            auto end = start + chunkSize;

            std::deque<packet::SequenceId> acksToSerialize(start, end);
            packet::internal::RichAck ack{.oobAcks = acksToSerialize};

            Packet p(static_cast<packet::Id>(packet::SystemMessageId::kRichAck),
                     packet::internal::RichAck::kFlag,
                     packet::internal::RichAck::kChannel);
            p << ack;

            uint32_t sequenceId = _localSequenceId++;
            rawSend(p, sequenceId, 0);

            processed += chunkSize;
        }

        _pendingRichAcks.clear();
    } else {
        LOG_TRACE_R2("Ahh it's empty, sending simple ACK...");

        Packet p(static_cast<packet::Id>(packet::SystemMessageId::kAck),
                 packet::Flag::kUnreliable,
                 packet::INTERNAL_CHANNEL_ID);
        uint32_t sequenceId = _localSequenceId++;

        rawSend(p, sequenceId, 0);
    }
}

void Session::_internal_disconnect() { this->_shouldClose = true; }

void Session::updateAcknowledgeInfo(uint32_t sequenceId)
{
    LOG_DEBUG("Updating acknowledge information");

    if (!_hasReceivedRemotePacket) {
        _remoteAcknowledgeId = sequenceId;
        _hasReceivedRemotePacket = true;
        _hasUnsentAck = true;
        return;
    }

    if (sequenceId > _remoteAcknowledgeId) {
        LOG_DEBUG("sequenceId > _remoteAcknowledgeId");
        uint32_t shift = sequenceId - _remoteAcknowledgeId;

        if (shift > 32) {
            for (uint32_t i = 0; i < 32; ++i) {
                if (_remoteAcknowledgeBitfield & (1U << i)) {
                    _oldPacketHistory.push_back(_remoteAcknowledgeId - (i + 1));
                }
            }
            _oldPacketHistory.push_back(_remoteAcknowledgeId);
            _remoteAcknowledgeBitfield = 0;
        } else {
            for (uint32_t i = 32 - shift; i < 32; ++i) {
                if (_remoteAcknowledgeBitfield & (1U << i)) {
                    _oldPacketHistory.push_back(_remoteAcknowledgeId - (i + 1));
                }
            }

            _remoteAcknowledgeBitfield <<= shift;
            _remoteAcknowledgeBitfield |= 1 << (shift - 1);
        }
        _remoteAcknowledgeId = sequenceId;
    } else if (sequenceId < _remoteAcknowledgeId) {
        uint32_t diff = _remoteAcknowledgeId - sequenceId;

        if (diff <= 32) {
            LOG_DEBUG("Diff is under 32, simple ACK");
            _remoteAcknowledgeBitfield |= 1U << (diff - 1);
        } else {
            LOG_DEBUG("Diff is greater than 32 ({}), pushing to history.", diff);
            auto it = std::ranges::find(_oldPacketHistory, sequenceId);

            if (it == _oldPacketHistory.end()) {  // if not in the history, then add it
                LOG_DEBUG("Not in the history, adding it to history.");
                _oldPacketHistory.push_back(sequenceId);
                if (_oldPacketHistory.size() > packet::MAX_PACKET_HISTORY_SIZE) {
                    _oldPacketHistory.pop_front();
                }
            }
        }
    }

    _hasUnsentAck = true;
}

bool Session::isDuplicate(uint32_t sequenceId) const
{
    if (!_hasReceivedRemotePacket) {
        return false;
    }

    if (sequenceId == _remoteAcknowledgeId) {
        return true;
    }

    if (sequenceId > _remoteAcknowledgeId) {
        return false;
    }

    uint32_t diff = _remoteAcknowledgeId - sequenceId;

    if (diff <= 32) {
        return (_remoteAcknowledgeBitfield & (1 << (diff - 1))) != 0;
    }

    const auto it = std::ranges::find(_oldPacketHistory, sequenceId);
    return (it != _oldPacketHistory.end());
}

void Session::checkForOldPackets(std::shared_ptr<ByteBuffer> rawData,
                                 const packet::Header& header)
{
    Packet incomingPacket(header.messageId, static_cast<packet::Flag>(header.flags));

    if (rawData->size() > sizeof(packet::Header)) {
        ByteBuffer payload;

        payload.assign(rawData->begin() + sizeof(packet::Header), rawData->end());
        incomingPacket._internal_setPayload(std::move(payload));

        try {
            packet::internal::RichAck richAck;
            incomingPacket >> richAck;

            if (!_sentPackets.empty()) {
                for (uint32_t ackedSeqId : richAck.oobAcks) {
                    if (_sentPackets.erase(ackedSeqId)) {
                        LOG_TRACE_R2("Packet #{} acknowledged via RICH_ACK.", ackedSeqId);
                    }
                }
            }
        } catch (const std::exception& e) {
            LOG_ERR("Failed to deserialize RICH_ACK packet: {}", e.what());
        }
    }
}

void Session::_updateRtt(const milliseconds rtt)
{
    const auto rttMs = static_cast<uint32_t>(rtt.count());
    _sessionMetrics.rtt.store(rttMs, std::memory_order_relaxed);

    const uint32_t currentMax = _sessionMetrics.maxRtt.load(std::memory_order_relaxed);
    if (rttMs > currentMax) {
        _sessionMetrics.maxRtt.store(rttMs, std::memory_order_relaxed);
    }
}

}  // namespace rtnt::core
