#pragma once

#include "rtecs/exceptions/BaseException.hpp"
#include "rtecs/types/types.hpp"

namespace rtecs::exceptions {

class ComponentAlreadyExists final : public BaseException
{
public:
    explicit ComponentAlreadyExists(types::ComponentID componentID);
};

}
