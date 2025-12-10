#include <ranges>

#include "logger/Logger.h"

#include "rtnt/Core/Session.hpp"
#include "rtnt/Common/Utils.hpp"

namespace rtnt::core
{

Session::Session(
    udp::endpoint endpoint,
    SendToPeerFunction sendToPeerFunction
)   : _endpoint(std::move(endpoint))
    , _sendToPeerFunction(std::move(sendToPeerFunction))
    , _lastSeen(steady_clock::now())
{}

bool Session::handleIncoming(
    const ByteBuffer &rawData,
    Packet &outPacket
)
{
    LOG_TRACE_R3(
        "Handling incoming raw data\n"
        "Size: {} bytes\n"
        "Data (N): {}",
        rawData.size(),
        byteBufferToHexString(rawData)
    );

    const packet::parsing::Result headerParsingResult = packet::Header::parse(rawData);

    if (!headerParsingResult) {
        LOG_TRACE_R3("Error while handling packet: {}", packet::parsing::to_string(headerParsingResult.error));
        return false;
    }

    const packet::Header& header = *headerParsingResult.header;

    _lastSeen = steady_clock::now();

    if (header.sequenceId > _remoteSequenceId) {
        _remoteSequenceId = header.sequenceId;
    }

    outPacket = Packet(
        header.messageId,
        static_cast<packet::Flag>(header.flags)
    );

    size_t payloadSize = rawData.size() - sizeof(packet::Header);

    if (payloadSize == 0) {
        return true;
    }

    ByteBuffer payload;
    payload.assign(
        rawData.begin() + sizeof(packet::Header),
        rawData.end()
    );
    outPacket._internal_setPayload(std::move(payload));
    return true;
}

void Session::send(Packet &packet)
{
    packet::Header header {};

    header.sequenceId = _localSequenceId++;
    header.acknowledgeId = _remoteSequenceId;
    header.acknowledgeBitfield = 0; // todo: Implement ack bitfield
    header.messageId = packet.getId();
    header.flags = static_cast<uint8_t>(packet.getReliability());
    header.packetSize = static_cast<uint16_t>(packet.getPayload().size());
    header.checksum = 0; // todo: Implement CRC32 checksum

    ByteBuffer rawBuffer;
    const auto& payload = packet.getPayload();

    rawBuffer.reserve(sizeof(packet::Header) + payload.size());

    header.toNetwork();
    const auto* headerPtr = reinterpret_cast<const uint8_t*>(&header);
    rawBuffer.insert(rawBuffer.end(), headerPtr, headerPtr + sizeof(packet::Header));
    rawBuffer.insert(rawBuffer.end(), payload.begin(), payload.end());
    header.toHost();

    LOG_TRACE_R3(
        "Preparing to send a packet.\n"
        "Sequence ID: {}\n"
        "Acknowledge ID: {}\n"
        "Acknowledge bitfield: {}\n"
        "Message ID: {}\n"
        "Flags: {}\n"
        "Payload Size: {}\n"
        "Checksum: {}\n"
        "Raw header (H): {}\n"
        "Raw buffer (N): {}",
        header.sequenceId,
        header.acknowledgeId,
        header.acknowledgeBitfield,
        header.messageId,
        header.flags,
        header.packetSize,
        header.checksum,
        byteBufferToHexString(rawBuffer.begin(), rawBuffer.begin() + sizeof(packet::Header)),
        byteBufferToHexString(rawBuffer.begin() + sizeof(packet::Header), rawBuffer.end())
    );

    if (_sendToPeerFunction) {
        _sendToPeerFunction(rawBuffer);
    }
}

}
