#include "rtecs/exceptions/UnknowComponentException.hpp"

using namespace rtecs::exceptions;

UnknowComponentException::UnknowComponentException(const types::ComponentID componentID)
    : BaseException("The component with ID '" + std::to_string(componentID) + "' does not exist.")
{
}
