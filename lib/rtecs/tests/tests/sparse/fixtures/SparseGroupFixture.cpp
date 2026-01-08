#include "SparseGroupFixture.hpp"

using namespace rtecs::tests::fixture;

void SparseGroupFixture::SetUp()
{
    _hitboxSet->put(0, {0, 0, 10, 10});
    _profilesSet->put(0, {"Carrot", 20});

    _hitboxSet->put(1, {5, 5, 10, 10});
    _healthSet->put(1, {20});

    _profilesSet->put(2, {"Potato", 21});
    _healthSet->put(2, {15});
}
