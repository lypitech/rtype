#include <utility>

#include "logger/Logger.h"

#include "rtnt/Core/Session.hpp"

namespace rtnt::core
{

Session::Session(
    udp::endpoint endpoint,
    SendToPeerFunction sendToPeerFunction
)   : _endpoint(std::move(endpoint))
    , _sendToPeerFunction(std::move(sendToPeerFunction))
    , _lastSeen(system_clock::now())
{}

bool Session::handleIncoming(
    const ByteBuffer &rawData,
    Packet &outPacket
)
{
    const packet::Header* header;
    size_t payloadSize = 0;

    LOG_TRACE_R3(
        "Handling incoming raw data\n"
        "Size: {} bytes\n"
        "Data: [{}]",
        rawData.size(), rawData
    );

    if (rawData.size() < sizeof(packet::Header)) {
        LOG_TRACE_R3("Data received is too small to contain a header, probably random internet noise. Skipping...");
        return false;
    }

    _lastSeen = system_clock::now();

    header = reinterpret_cast<const packet::Header*>(rawData.data());

    if (header->protocolId != PROTOCOL_ID) {
        LOG_TRACE_R2("Sender protocol ID doesn't match the local protocol ID.");
        return false;
    }

    if (header->sequenceId > _remoteSequenceId) {
        _remoteSequenceId = header->sequenceId;
    }

    outPacket = Packet(
        header->messageId,
        static_cast<packet::Flag>(header->flags)
    );

    payloadSize = rawData.size() - sizeof(packet::Header);

    if (payloadSize == 0) {
        return true;
    }

    if (payloadSize != header->packetSize) {
        LOG_TRACE_R2("Raw data size doesn't match the packet size (corrupted packet).");
        return false;
    }

    ByteBuffer payload;

    payload.assign(
        rawData.begin() + sizeof(packet::Header),
        rawData.end()
    );
    outPacket._internal_setPayload(payload);
    return true;
}

void Session::send(Packet &packet)
{
    packet::Header header {};

    header.protocolId = htons(PROTOCOL_ID);
    header.sequenceId = htonl(_localSequenceId++);
    header.acknowledgeId = htonl(_remoteSequenceId);
    header.acknowledgeBitfield = 0; // todo: Implement ack bitfield
    header.messageId = htons(packet.getId());
    header.flags = static_cast<uint8_t>(packet.getReliability());
    header.packetSize = htons(static_cast<uint16_t>(packet.getPayload().size()));
    header.checksum = 0; // todo: Implement CRC32 checksum

    ByteBuffer rawBuffer;
    const auto& payload = packet.getPayload();

    rawBuffer.reserve(sizeof(packet::Header) + payload.size());

    const auto* headerPtr = reinterpret_cast<const uint8_t*>(&header);
    rawBuffer.insert(rawBuffer.end(), headerPtr, headerPtr + sizeof(packet::Header));
    rawBuffer.insert(rawBuffer.end(), payload.begin(), payload.end());

    LOG_TRACE_R3(
        "Preparing to send a packet.\n"
        "Sequence ID: {}\n"
        "Acknowledge ID: {}\n"
        "Acknowledge bitfield: {}\n"
        "Message ID: {}\n"
        "Flags: {}\n"
        "Payload Size: {}\n"
        "Checksum: {}\n"
        "Buffer: [{}]",
        header.sequenceId,
        header.acknowledgeId,
        header.acknowledgeBitfield,
        header.messageId,
        header.flags,
        header.packetSize,
        header.checksum,
        rawBuffer
    );

    if (_sendToPeerFunction) {
        _sendToPeerFunction(rawBuffer);
    }
}

}
