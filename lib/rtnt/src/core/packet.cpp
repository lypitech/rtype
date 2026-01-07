#include "rtnt/core/packet.hpp"

namespace rtnt::core::packet {

Flag operator&(Flag lhs,
               Flag rhs)
{
    return static_cast<Flag>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

using namespace parsing;

Result Header::parse(const ByteBuffer& data)
{
    if (data.size() < sizeof(Header)) {
        return Result::failure(Error::kDataTooSmall);
    }

    Header header;
    std::memcpy(&header, data.data(), sizeof(Header));
    header.convertEndianness();

    if (header.protocolId != PROTOCOL_ID) {
        return Result::failure(Error::kProtocolMismatch);
    }

    if (data.size() - sizeof(Header) < header.packetSize) {
        return Result::failure(Error::kPayloadSizeMismatch);
    }

    return Result::success(header);
}

}  // namespace rtnt::core::packet
