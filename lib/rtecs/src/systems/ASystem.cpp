#include "rtecs/systems/ASystem.hpp"

using namespace rtecs::systems;

ASystem::ASystem(const std::string& name)
    : _kName(name)
{
}

const std::string& ASystem::getName() { return _kName; }
