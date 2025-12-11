#pragma once

#include <format>
#include <vector>

#if defined(_WIN32)
    #include <winsock2.h>
#elif defined(__linux__)
    #include <arpa/inet.h>
    #include <cstring>
#endif

#include "logger/Logger.h"
#include "rtnt/Common/Constants.hpp"

namespace rtnt::core
{

class Packet; // Forward declaration needed for packet::Reader
class Session;

using ByteBuffer = std::vector<uint8_t>;

namespace packet
{

    using Id = uint16_t;
    using Name = std::string_view;

    /**
     * @enum    packet::Flag
     * @brief   Flags defining the behavior of a packet delivery.
     */
    enum class Flag : uint8_t
    {
        kUnreliable = 1 << 0, ///< Fire and forget. May be lost or arrive out of order.
        kReliable   = 1 << 1, ///< Guaranteed delivery. Will be resent until ACKed.
        kOrdered    = 1 << 2, ///< Guaranteed order. Will be buffered until previous packets arrive.
    };

    struct Header;

    namespace parsing
    {

        struct Result;

        /**
         * @enum    packet::parsing::Error
         * @brief   Errors that can occur during Header parsing.
         */
        enum class Error : uint8_t
        {
            kNone = 0x0,            ///< No error during parsing.
            kDataTooSmall,          ///< Data is too small to contain a RTNT header.
            kProtocolMismatch,      ///< Protocol ID received does not match local RTNT protocol ID.
            kPayloadSizeMismatch,   ///< Corrupted packet: Payload size does not match the one written in the header.
        };

        /**
         * @param   error   Error
         * @return  Converted string
         */
        inline std::string_view to_string(Error error)
        {
            switch (error) {
                case Error::kNone:                  return "Success";
                case Error::kDataTooSmall:          return "Data is too small to contain a RTNT header.";
                case Error::kProtocolMismatch:      return "Protocol ID mismatch.";
                case Error::kPayloadSizeMismatch:   return "Corrupted packet: Payload size does not match header.";
                default:                            return "Unknown error.";
            }
        }

    }

    #pragma pack(push, 1) // forcing strict alignment, no compiler padding
    /**
     * @struct  packet::Header
     * @brief   The RUDP Wire Header.
     * @note    This struct is packed (1-byte alignment) to ensure consistent binary layout across platforms.
     * @warning All multibyte fields MUST be converted to Network Byte Order (Big Endian) before sending
     * (cf. toNetwork and toHost).
     */
    struct Header final
    {
        uint16_t protocolId             = PROTOCOL_ID;                              ///< Magic number representig unique ID of the protocol, to avoid internet noise
        uint16_t protocolVersion        = PROTOCOL_VER;                             ///< Protocol version, to reject mismatch peers
        uint32_t sequenceId             = 0;                                        ///< The unique, incrementing ID of this packet
        uint32_t acknowledgeId          = 0;                                        ///< Sequence ID of the latest packet received
        uint32_t acknowledgeBitfield    = 0;                                        ///< Bitmask of the previous 32 received packets relative to acknowledge ID
        Id       messageId              = 0x0;                                      ///< Command type (user-defined)
        uint8_t  flags                  = static_cast<uint8_t>(Flag::kUnreliable);  ///< Reliability flags (cf. packet::Flag)
        uint16_t packetSize             = 0;                                        ///< Size of the payload
        uint32_t checksum               = 0;                                        ///< CRC32 checksum to avoid corruption

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

        /**
         * @brief   Tries to parse a
         * @param   data    Raw buffer data
         * @return  A @code parsing::Result@endcode instance with details in it.
         *          If an error occurred during parsing, @code parsing::Result::header@endcode will be set to
         *          @code std::nullopt@endcode and @code parsing::Result::header@endcode will be set to something
         *          different from @code parsing::Error::kNone@endcode.
         */
        static parsing::Result parse(const ByteBuffer& data);
    };
    #pragma pack(pop)

    namespace parsing
    {

        /**
         * @struct  Result
         * @brief   Container for Header parsing result.
         *
         * Used for Header parsing, this class allows
         */
        struct Result final
        {
            std::optional<Header> header;
            Error error;

            /**
             * @return  @code true@endcode if parsing successfully performed
             */
            explicit operator bool() const { return error == Error::kNone; }

            /**
             * @brief   Constructs a Result for a successful parsing.
             * @param   header  Valid header to put in the result
             * @return  Successful result
             */
            static Result success(Header header) { return { header, Error::kNone }; }

            /**
             * @brief   Constructs a Result for a failed parsing.
             * @param   error   Error that occurred during parsing
             * @return  Failed result
             */
            static Result failure(Error error)   { return { std::nullopt, error }; }
        };

    }

    /**
     * @struct  packet::Reader
     * @brief   Helper class that behaves like a Packet but reads instead of writing.
     * This allows to use the '&' operator for reading.
     */
    struct Reader final
    {
        Packet& _packet;

        template <typename T>
        Reader& operator&(T& data);
    };

    /**
     * @tparam  T   Packet struct
     * @return  The name that is contained in the packet struct.
     *          If there is no name, @code UNKNOWN_PACKET_NAME@endcode will be returned.
     */
    template <typename T>
    constexpr std::string_view getName()
    {
        if constexpr (requires { T::kName; }) {
            return T::kName;
        } else {
            return UNKNOWN_PACKET_NAME;
        }
    }

    /**
     * @tparam  T   Packet struct
     * @return  The flag that is contained in the packet struct.
     *          Default to @code Flag::kUnreliable@endcode.
     */
    template <typename T>
    constexpr Flag getFlag()
    {
        if constexpr (requires { T::kFlag; }) {
            return T::kFlag;
        } else {
            return Flag::kUnreliable;
        }
    }

    /**
     * @brief   Verifies if a given struct has the layout of a packet.
     *
     * This function checks if:
     * - Given typename is a struct or a class.
     * - There is a @code kId@endcode field in the struct.
     * - The @code kId@endcode field is a @code uint16_t@endcode (@code Id@endcode)
     *
     * It also checks if a name is present in the given struct. If not, a warning will be logged.
     *
     * @tparam  T   Packet struct to verify
     */
    template <typename T>
    void verifyPacketData()
    {
        static_assert(
            std::is_class_v<T>,
            "Packet data must be a struct."
        );

        static_assert(
            requires { T::kId; },
            "Packet struct is missing 'static constexpr uint16_t kId'."
        );

        using IdType = decltype(T::kId);

        static_assert(
            std::is_same_v<const Id, IdType>,
            "Packet kId must be a 16-bit unsigned integer (uint16_t)."
        );

        if constexpr (!requires { T::kName; }) {
            LOG_WARN(
                "Warning for Packet #{}: "
                "It is strongly recommended to give a name to the packets you define. "
                "Fallback to \"{}\".",
                T::kId,
                UNKNOWN_PACKET_NAME
            );
        }
    }

    /**
     * @brief   Verifies if a given struct has the layout of a user-defined packet.
     *
     * It acts the same as @code verifyPacketData@endcode, but also checks for the ID.
     * If the ID is less than 128 (0-128 is reserved to internal packets), a compilation error will be thrown.
     *
     * @tparam  T   Packet struct to verify
     */
    template <typename T>
    void verifyUserPacketData()
    {
        verifyPacketData<T>();

        static_assert(
            static_cast<const Id>(T::kId) >= 128, // fixme: fix magic number
            "User-defined packet IDs must be >= 128."
        );
    }

    /**
     * @brief   Verifies if a given struct has the layout of a user-defined packet.
     *
     * It acts the same as @code verifyPacketData@endcode, but also checks for the ID.
     * If the ID is equal or greater than 128 (128-65535 is reserved to user-defined packets), a compilation error will
     * be thrown.
     *
     * @tparam  T   Packet struct to verify
     */
    template <typename T>
    void verifyInternalPacketData()
    {
        verifyPacketData<T>();

        static_assert(
            static_cast<const Id>(T::kId) < 128, // fixme: fix magic number
            "Internal packet IDs must be < 128."
        );
    }

    /**
     * @brief   Verifies if a given struct has an integrated callback.
     *
     * More precisely, it verifies that the struct contains a function that have this signature:
     * @code static void onReceive(const std::shared_ptr<Session>& session, const T& packet)@endcode.
     *
     * @tparam  T   Packet struct to verify
     */
    template <typename T>
    void verifyPacketIntegratedCallback()
    {
        static_assert(
            requires(std::shared_ptr<Session> s, const T& t) { T::onReceive(s, t); },
            "Packet is missing 'static void onReceive(std::shared_ptr<Session>, const T&)'"
        );
    }

    /**
     * @tparam  T       Packet struct to compare the ByteBuffer to
     * @param   rawData Buffer of raw bytes to compare
     * @return  @code true@endcode if given ByteBuffer contains a valid T header
     */
    template <typename T>
    bool is(const ByteBuffer& rawData)
    {
        verifyPacketData<T>();
        parsing::Result res = Header::parse(rawData);
        return res && res.header->messageId == T::kId;
    }

}

/**
 * @class   Packet
 * @brief   A dynamic buffer wrapper for serializing and deserializing data.
 *
 * The Packet class acts as a stream. You can write data into it using the @code <<@endcode operator
 * and read data from it using the @code >>@endcode operator. It handles endianness for internal
 * headers but assumes payload data is handled by the user (or is POD, Plain Old Data).
 */
class Packet final
{
public:
    /**
     * @brief   Constructs a new Packet
     * @param   id          The user-defined message ID
     * @param   flag        Reliability mode
     * @param   channelId   Virtual channel ID // todo: implement channel id lol
     */
    explicit Packet(
        const packet::Id id,
        const packet::Flag flag = packet::Flag::kUnreliable,
        const uint8_t channelId = 0
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

    [[nodiscard]] uint16_t getId() const { return _messageId; }
    [[nodiscard]] packet::Flag getReliability() const { return _flag; }
    [[nodiscard]] uint8_t getChannel() const { return _channelId; }
    [[nodiscard]] const std::vector<uint8_t>& getPayload() const { return _buffer; }

private:
    friend class Session;

    // Metadata
    uint16_t _messageId;
    packet::Flag _flag;
    uint8_t _channelId;

    // Data
    ByteBuffer _buffer;
    size_t _readPosition = 0;

    void _internal_setPayload(std::vector<uint8_t>&& data)
    {
        _buffer = std::move(data);
        _readPosition = 0;
    }

    /**
     * @brief   Pushes raw data to the buffer
     * @param   data    Data to push
     * @param   size    Size of the data to push in bytes
     */
    void append(
        const void* data,
        const size_t size
    )
    {
        const auto* ptr = static_cast<const uint8_t*>(data);

        _buffer.insert(_buffer.end(), ptr, ptr + size);
    }
};

}
