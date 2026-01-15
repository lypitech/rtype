#include "rtnt/common/utils.hpp"

#include <format>

namespace rtnt {

std::string byteBufferToHexString(const core::ByteBuffer::const_iterator begin,
                                  const core::ByteBuffer::const_iterator end)
{
    const size_t size = std::distance(begin, end);

    if (size == 0) {
        return "[]";
    }

    std::string result;
    result.reserve(1 + size * 3 - 1 + 1);

    result += '[';
    for (auto it = begin; it != end; ++it) {
        if (it != begin) {
            result += ' ';
        }
        result += std::format(
            "{:02X}",
            *it);  // todo: optimization: manual hex conversion instead of calling performance-heavy std::format.
    }
    result += ']';

    return result;
}

std::string byteBufferToHexString(const core::ByteBuffer &buffer)
{
    return byteBufferToHexString(buffer.begin(), buffer.end());
}

std::string bitfieldToString(const uint32_t bitfield)
{
    std::string res;

    for (uint8_t k = 0; k < 32; k++) {
        if (bitfield & (1 << k)) {
            res += "•";
        } else {
            res += "◦";
        }
    }
    return res;
}

}  // namespace rtnt
