#pragma once

#include "SparseFixture.hpp"

namespace rtecs::tests::fixture {

/**
 * @brief This fixture extends from SparseFixture.
 *
 * @note On its setup, it creates the following entities :
 * - 0: hitbox & profile
 * - 1: hitbox & health
 * - 2: profile & health
 */
class SparseGroupFixture : public SparseFixture
{
    /**
     * @brief Setups the following entities :
     * - 0: hitbox & profile
     * - 1: hitbox & health
     * - 2: profile & health
     */
    void SetUp() override;
};

}  // namespace rtecs::tests::fixture
