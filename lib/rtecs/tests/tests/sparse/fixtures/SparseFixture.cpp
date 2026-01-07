#include "SparseFixture.hpp"

SparseFixture::SparseFixture()
    : _profilesSet(std::make_unique<sparse::SparseSet<Profile>>(0)),
      _damageablesSet(std::make_unique<sparse::SparseSet<Damageable>>(1)),
      _collidableSet(std::make_unique<sparse::SparseSet<Collidable>>(2))
{
}
