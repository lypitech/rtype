#pragma once

#include "rtnt/core/packet.hpp"

namespace rtnt {

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
