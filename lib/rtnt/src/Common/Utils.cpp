#include "rtnt/Common/Utils.hpp"

namespace rtnt
{

std::string byteBufferToHexString(
    const core::ByteBuffer::const_iterator begin,
    const core::ByteBuffer::const_iterator end
)
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
        result += std::format("{:02X}", *it); // todo: optimization: manual hex conversion instead of calling performance-heavy std::format.
    }
    result += ']';

    return result;
}

std::string byteBufferToHexString(const core::ByteBuffer &buffer)
{
    return byteBufferToHexString(buffer.begin(), buffer.end());
}

}
