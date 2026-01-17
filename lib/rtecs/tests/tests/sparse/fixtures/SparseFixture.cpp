#include "SparseFixture.hpp"

using namespace rtecs::tests::fixture;

SparseFixture::SparseFixture()
    : _profilesSet(std::make_shared<sparse::SparseSet<Profile>>(0)),
      _healthSet(std::make_shared<sparse::SparseSet<Health>>(1)),
      _hitboxSet(std::make_shared<sparse::SparseSet<Hitbox>>(2))
{
}
