#include "rtecs/exceptions/BaseException.hpp"

using namespace rtecs::exceptions;

BaseException::BaseException(const std::string &kMessage)
    : _kMessage(kMessage)
{
}

const char *BaseException::what() const noexcept { return ("[RTECS::ERROR] " + _kMessage).data(); }
