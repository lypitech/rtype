#include "rtecs/exceptions/EntityNotFoundException.hpp"

using namespace rtecs::exceptions;

EntityNotFoundException::EntityNotFoundException(const types::EntityID entityID, const types::ComponentID componentID)
    : BaseException("The entity with ID '" + std::to_string(entityID) + "' does not exist in the component with ID '" +
                    std::to_string(componentID) + "'.")
{
}
