#include "rtnt/Core/Packet.hpp"

namespace rtnt::core {

namespace packet {

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

template <typename T>
Reader& Reader::operator&(T& data)
{
    _packet >> data;  // Redirects '&' to Packet's read logic
    return *this;
}

}  // namespace packet

/**
 * @brief Global operator to WRITE a custom struct into a packet.
 */
template <typename T>
std::enable_if<!std::is_standard_layout<T>::value, Packet&>::type operator<<(Packet& p, const T& data)
{
    const_cast<T&>(data).serialize(p);
    return p;
}

/**
 * @brief Global operator to READ a custom struct from a packet.
 *
 * @param   p       Packet to read from
 * @param   data    a
 */
template <typename T>
std::enable_if<!std::is_standard_layout<T>::value, Packet&>::type operator>>(Packet& p, T& data)
{
    packet::Reader reader{p};

    data.serialize(reader);
    return p;
}

}  // namespace rtnt::core
