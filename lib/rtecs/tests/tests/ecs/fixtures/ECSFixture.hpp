#pragma once

#include "../../fixtures/ComponentFixture.hpp"
#include "rtecs/ECS.hpp"
#include "rtecs/systems/ASystem.hpp"

namespace rtecs::tests::fixture {

/**
 * @brief This fixture offers and ECS with pre-registered components, pre-registered entities and pre-registered systems :
 * -@code Components @endcode: Profile, Hitbox, Health
 * -@code Systems @endcode:
 *   -@code RenameOnLowLifeSystem @endcode:@code Health & Profile @endcode
 *   -@code DamageOnCollideSystem @endcode:@code Hitbox & Health @endcode
 *   -@code LogOnCollideSystem @endcode:@code Profile & Hitbox @endcode
 *   -@code MoveToCenterSystem @endcode:@code Hitbox @endcode
 */
class ECSFixture : public ComponentFixture
{
    /**
     * @brief This system reduce entities' health when they collide with another entity.
     */
    class DamageOnCollideSystem final : public systems::ASystem
    {
    public:
        explicit DamageOnCollideSystem();
        void apply(ECS& ecs) override;
    };

    /**
     * @brief This system moves entities to the center of the map (0, 0) with a speed of 5.
     *
     * @important Once they are at (0, 0), they don't move anymore.
     */
    class MoveToCenterSystem final : public systems::ASystem
    {
    public:
        explicit MoveToCenterSystem();
        void apply(ECS& ecs) override;
    };

protected:
    ECS _ecs;

    /**
     * Initialize the ECS with some pre-registered components, pre-registered entities and pre-registered systems.
     */
    void SetUp() override;

    /**
     * Destroy the ECS.
     */
    void TearDown() override;
};

}  // namespace rtecs::tests::fixture
