#pragma once

#include <algorithm>
#include <array>
#include <bit>

#include "rtnt/common/constants.hpp"

namespace rtnt {

namespace endian {

template <typename T>
T swap(T value)
{
    if constexpr (sizeof(T) == 1) {  // 1-byte types don't need swapping (what do you wanna swap)
        return value;
    }

    if constexpr (std::endian::native ==
                  std::endian::big) {  // no need to swap if already big endian
        return value;
    }

    // reverse bytes
    auto bytes = std::bit_cast<std::array<uint8_t, sizeof(T)>>(value);
    std::ranges::reverse(bytes);
    return std::bit_cast<T>(bytes);
}

}  // namespace endian

/**
 * @brief   Converts a section of a ByteBuffer to a readable string in a hexadecimal form.
 * Mainly used for logging.
 *
 * @note    Only goes from begin to end. If you want to convert an entire ByteBuffer, simply call the other
            overload.
 * @note    This function doesn't affect the buffer.
 *
 * @param   begin   Starting iterator (from)
 * @param   end     Ending iterator (to)
 * @return  Converted buffer
 */
std::string byteBufferToHexString(core::ByteBuffer::const_iterator begin,
                                  core::ByteBuffer::const_iterator end);

/**
 * @brief   Converts a ByteBuffer to a readable string in a hexadecimal form.
 * Mainly used for logging.
 *
 * @note    This function doesn't affect the buffer.
 *
 * @param   buffer      Buffer to convert
 * @return  Converted buffer
 */
std::string byteBufferToHexString(const core::ByteBuffer& buffer);

}  // namespace rtnt
