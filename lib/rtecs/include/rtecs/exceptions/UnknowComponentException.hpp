#pragma once

#include "rtecs/exceptions/BaseException.hpp"
#include "rtecs/types/types.hpp"

namespace rtecs::exceptions {

class UnknowComponentException final : public BaseException
{
public:
    explicit UnknowComponentException(types::ComponentID componentID);
};

}  // namespace rtecs::exceptions
