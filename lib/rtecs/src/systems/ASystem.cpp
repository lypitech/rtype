#include "rtecs/systems/ASystem.hpp"

using namespace rtecs::systems;

ASystem::ASystem(const bitset::DynamicBitSet& mask)
    : _mask(mask)
{}

const rtecs::bitset::DynamicBitSet& ASystem::getMask() const noexcept
{
    return _mask;
}
