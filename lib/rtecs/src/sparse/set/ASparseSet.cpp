#include "rtecs/sparse/set/ASparseSet.hpp"

using namespace rtecs::sparse;

ASparseSet::ASparseSet(const types::ComponentID id)
    : _id(id)
{
}

const std::vector<rtecs::types::EntityID>& ASparseSet::getEntities() const noexcept
{
    return _entities;
}

rtecs::types::ComponentID ASparseSet::getId() const { return _id; }
