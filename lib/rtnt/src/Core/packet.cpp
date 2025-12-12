#include "rtnt/Core/packet.hpp"

namespace rtnt::core::packet {

using namespace parsing;

Result Header::parse(const ByteBuffer& data)
{
    if (data.size() < sizeof(Header)) {
        return Result::failure(Error::kDataTooSmall);
    }

    Header header;
    std::memcpy(&header, data.data(), sizeof(Header));
    header.toHost();

    if (header.protocolId != PROTOCOL_ID) {
        return Result::failure(Error::kProtocolMismatch);
    }

    if (data.size() - sizeof(Header) < header.packetSize) {
        return Result::failure(Error::kPayloadSizeMismatch);
    }

    return Result::success(header);
}

} // namespace rtnt::core::packet
