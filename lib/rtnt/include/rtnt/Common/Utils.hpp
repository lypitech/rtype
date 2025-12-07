#pragma once

#include "rtnt/Core/Packet.hpp"

namespace rtnt
{

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

}
