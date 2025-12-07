#pragma once

#include <format>
#include <vector>

#include "rtnt/Common/Constants.hpp"

namespace rtnt::core
{

class Packet; // Forward declaration needed for packet::Reader

namespace packet
{

    /**
     * @enum    packet::Flag
     * @brief   Flags defining the behavior of a packet delivery.
     */
    enum class Flag : uint8_t
    {
        kUnreliable     = 1 << 0, ///< Fire and forget. May be lost or arrive out of order.
        kReliable       = 1 << 1, ///< Guaranteed delivery. Will be resent until ACKed.
        kSynchronized   = 1 << 2, ///< Guaranteed order. Will be buffered until previous packets arrive.
    };

    #pragma pack(push, 1) // forcing strict alignment, no compiler padding
    /**
     * @struct  packet::Header
     * @brief   The RUDP Wire Header.
     * @note    This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
     * @warning All multibyte fields MUST be converted to Network Byte Order (Big Endian) before sending
     * (cf. toNetwork and toHost).
     */
    struct Header
    {
        uint16_t protocolId             = PROTOCOL_ID; ///< Magic number representig unique ID of the protocol, to avoid internet noise
        uint16_t protocolVersion        = PROTOCOL_VER; ///< Protocol version, to reject mismatch peers
        uint32_t sequenceId             = 0; ///< The unique, incrementing ID of this packet
        uint32_t acknowledgeId          = 0; ///< Sequence ID of the latest packet received
        uint32_t acknowledgeBitfield    = 0; ///< Bitmask of the previous 32 received packets relative to acknowledge ID
        uint16_t messageId              = 0x0; ///< Command type (user-defined)
        uint8_t  flags                  = static_cast<uint16_t>(Flag::kUnreliable); ///< Reliability flags (cf. packet::Flag)
        uint16_t packetSize             = 0; ///< Size of the payload
        uint32_t checksum               = 0; ///< CRC32 checksum to avoid corruption

        /**
         * @brief   Converts all fields from Host Byte Order (Little Endian)
         *          to Network Byte Order (Big Endian) IN PLACE.
         */
        void toNetwork()
        {
            protocolId          = htons(protocolId);
            protocolVersion     = htons(protocolVersion);
            sequenceId          = htonl(sequenceId);
            acknowledgeId       = htonl(acknowledgeId);
            acknowledgeBitfield = htonl(acknowledgeBitfield);
            messageId           = htons(messageId);
            // flags is uint8_t, no conversion needed
            packetSize          = htons(packetSize);
            checksum            = htonl(checksum);
        }

        /**
         * @brief   Converts all fields from Network Byte Order (Big Endian)
         *          to Host Byte Order (Little Endian) IN PLACE.
         */
        void toHost()
        {
            protocolId          = ntohs(protocolId);
            protocolVersion     = ntohs(protocolVersion);
            sequenceId          = ntohl(sequenceId);
            acknowledgeId       = ntohl(acknowledgeId);
            acknowledgeBitfield = ntohl(acknowledgeBitfield);
            messageId           = ntohs(messageId);
            // flags is uint8_t, no conversion needed
            packetSize          = ntohs(packetSize);
            checksum            = ntohl(checksum);
        }
    };
    #pragma pack(pop)

    /**
     * @struct  packet::Reader
     * @brief   Helper class that behaves like a Packet but Reads instead of Writing.
     * This allows to use the '&' operator for reading.
     */
    struct Reader final
    {
        Packet& _packet;

        template <typename T>
        Reader& operator&(T& data);
    };

}

using ByteBuffer = std::vector<uint8_t>;

/**
 * @class   Packet
 * @brief   A dynamic buffer wrapper for serializing and deserializing data.
 *
 * The Packet class acts as a stream. You can write data into it using the @code<< @endcode operator
 * and read data from it using the @code>> @endcode operator. It handles endianness for internal
 * headers but assumes payload data is handled by the user (or is POD, Plain Old Data).
 */
class Packet
{
public:
    /**
     * @brief   Constructs a new Packet
     * @param   id          The user-defined message ID
     * @param   flag        Reliability mode
     * @param   channelId   Virtual channel ID
     */
    explicit Packet(
        uint16_t id,
        packet::Flag flag = packet::Flag::kUnreliable,
        uint8_t channelId = 0
    )   : _messageId(id)
        , _flag(flag)
        , _channelId(channelId)
    {}

    /* Serializing methods */
    /**
     * @brief   Serializes a POD (Plain Old Data) type into the packet.
     * @tparam  T   Type of data to write (@code int@endcode, @code float@endcode, @code struct@endcode, etc.)
     * @note    This function is disabled for complex types (like @code std::string@endcode) to prevent unsafe
     * memory copying. For strings, see the dedicated operator.
     */
    template <typename T>
    std::enable_if<std::is_trivially_copyable<T>::value, Packet&>::type operator<<(const T& data)
    {
        append(&data, sizeof(T));
        return *this;
    }

    /**
     * @brief   Specialization to safely write @code std::string@endcode.
     * Writes a 2-byte length prefix followed by the characters.
     */
    Packet& operator<<(const std::string& str)
    {
        auto size = static_cast<uint16_t>(str.size());

        *this << size;
        append(str.data(), size);
        return *this;
    }
    /* --------------------- */

    /* Deserializing methods */
    /**
     * @brief   Deserializes a POD (Plain Old Data) type from the packet.
     * @tparam  T   Type of data to read (@code int@endcode, @code float@endcode, @code struct@endcode, etc.)
     * @note    This function is disabled for complex types (like @code std::string@endcode) to prevent unsafe
     * memory copying. For strings, see the dedicated operator.
     */
    template <typename T>
    std::enable_if<std::is_trivially_copyable<T>::value, Packet&>::type operator>>(T& data)
    {
        if (_readPosition + sizeof(T) > _buffer.size()) {
            throw std::runtime_error("Packet Underflow");
        }

        std::memcpy(&data, _buffer.data() + _readPosition, sizeof(T));
        _readPosition += sizeof(T);
        return *this;
    }

    /**
     * @brief   Specialization to safely read @code std::string@endcode.
     * Reads a 2-byte length prefix followed by the characters.
     */
    Packet& operator>>(std::string& str)
    {
        uint16_t size = 0;

        *this >> size;
        if (_readPosition + size > _buffer.size()) {
            throw std::runtime_error("Packet Underflow");
        }

        str.assign(
            _buffer.begin() + static_cast<long>(_readPosition),
            _buffer.begin() + static_cast<long>(_readPosition) + size
        );
        _readPosition += size;
        return *this;
    }
    /* ------------------- */

    template <typename T>
    Packet& operator&(const T& data)
    {
        return *this << data;
    }

    void _internal_setPayload(std::vector<uint8_t>&& data)
    {
        _buffer = data;
        _readPosition = 0;
    }

    [[nodiscard]] uint16_t getId() const { return _messageId; }
    [[nodiscard]] packet::Flag getReliability() const { return _flag; }
    [[nodiscard]] uint8_t getChannel() const { return _channelId; }
    [[nodiscard]] const std::vector<uint8_t>& getPayload() const { return _buffer; }

private:
    // Metadata
    uint16_t _messageId;
    packet::Flag _flag;
    uint8_t _channelId;

    // Data
    ByteBuffer _buffer;
    size_t _readPosition = 0;

    /**
     * @brief   Pushes raw data to the buffer
     * @param   data    Data to push
     * @param   size    Size of the data to push in bytes
     */
    void append(
        const void* data,
        size_t size
    )
    {
        const auto* ptr = static_cast<const uint8_t*>(data);

        _buffer.insert(_buffer.end(), ptr, ptr + size);
    }
};

}
