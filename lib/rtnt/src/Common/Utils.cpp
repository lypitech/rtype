#include "rtnt/Common/Utils.hpp"

namespace rtnt
{

std::string byteBufferToHexString(const core::ByteBuffer &buffer)
{
    std::string result = "[";
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (i > 0) {
            result += " ";
        }
        result += std::format("{:02X}", buffer[i]);
    }
    result += "]";
    return result;
}

}
