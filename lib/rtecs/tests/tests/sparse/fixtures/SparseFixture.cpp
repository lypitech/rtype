#include "SparseFixture.hpp"

using namespace rtecs::tests::fixture;

SparseFixture::SparseFixture()
    : _profilesSet(std::make_unique<sparse::SparseSet<Profile>>(0)),
      _healthSet(std::make_unique<sparse::SparseSet<Health>>(1)),
      _hitboxSet(std::make_unique<sparse::SparseSet<Hitbox>>(2))
{
}
