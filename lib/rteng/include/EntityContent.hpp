#pragma once

#include <algorithm>
#include <bit>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief   Swaps bytes from/to big/little endian.
 * @tparam  T       Data type
 * @param   value   Data to swap
 * @return  Swapped data
 */
template <typename T>
T swap(T value)
{
    if constexpr (sizeof(T) == 1) {  // 1-byte types don't need swapping (what do you wanna swap)
        return value;
    }

    // no need to swap if already big endian
    if constexpr (std::endian::native == std::endian::big) {
        return value;
    }

    // reverse bytes
    auto bytes = std::bit_cast<std::array<uint8_t, sizeof(T)>>(value);
    std::ranges::reverse(bytes);
    return std::bit_cast<T>(bytes);
}

class EntityContent
{
public:
    /* Serializing methods */
    /**
     * @brief   Serializes a POD (Plain Old Data) type into the packet.
     * @tparam  T   Type of data to write (@code int@endcode, @code float@endcode, @code struct@endcode, etc.)
     * @note    This function is disabled for complex types (like @code std::string@endcode) to prevent unsafe
     * memory copying. For strings, see the dedicated operator.
     */
    template <typename T>
    std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T>,
                     EntityContent&>
    operator<<(const T& data)
    {
        T nData = swap(data);
        append(&nData, sizeof(T));
        return *this;
    }

    /**
     * @brief   Specialization to safely write @code std::string@endcode.
     * Writes a 2-byte length prefix followed by the characters.
     * @param   str String to write
     */
    EntityContent& operator<<(const std::string& str)
    {
        const auto size = static_cast<uint16_t>(str.size());

        *this << size;
        append(str.data(), size);
        return *this;
    }

    /**
     * @brief   Pushes raw data to the buffer
     * @param   data    Data to push
     * @param   size    Size of the data to push in bytes
     */
    void append(const void* data,
                const size_t size)
    {
        const auto* ptr = static_cast<const uint8_t*>(data);

        _buffer.insert(_buffer.end(), ptr, ptr + size);
    }

    template <typename T>
    EntityContent& operator&(const T& data)
    {
        return *this << data;
    }

    std::vector<uint8_t> getData() { return _buffer; };

private:
    std::vector<uint8_t> _buffer;
};

/**
 * @brief Global operator to WRITE a custom struct into a packet.
 */
template <typename T>
std::enable_if_t<!std::is_arithmetic_v<T> && !std::is_enum_v<T>,
                 EntityContent&>
operator<<(EntityContent& p,
           const T& data)
{
    if constexpr (std::is_empty_v<T>) {
        return p;
    } else {
        const_cast<T&>(data).serialize(p);
        return p;
    }
}

/**
 * @brief       Specialization to safely write @code std::vector@endcode.
 * @warning     T MUST be serializable by rtnt (no complex types).
 * @tparam      T       Type of data that is contained in the vector
 * @param       p       Packet to write into
 * @param       data    Const reference to the vector to write
 */
template <typename T>
EntityContent& operator<<(EntityContent& p,
                          const std::vector<T>& data)
{
    if (data.size() > (std::numeric_limits<uint16_t>::max)()) {
        throw std::runtime_error("Vector is too large to serialize (limit 65535)");
    }

    const auto size = static_cast<uint16_t>(data.size());
    p << size;

    for (const auto& element : data) {
        p << element;
    }
    return p;
}
