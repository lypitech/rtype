#pragma once

#include "rtecs/exceptions/BaseException.hpp"
#include "rtecs/types/types.hpp"

namespace rtecs::exceptions {

class EntityNotFoundException final : public BaseException
{
public:
    explicit EntityNotFoundException(types::EntityID entityID, types::ComponentID componentID);
};

}  // namespace rtecs::exceptions
