#include "rtecs/exceptions/ComponentAlreadyExists.hpp"

using namespace rtecs::exceptions;

ComponentAlreadyExists::ComponentAlreadyExists(const types::ComponentID componentID)
    : BaseException("The component with ID '" + std::to_string(componentID) + "' already exists.")
{
}
