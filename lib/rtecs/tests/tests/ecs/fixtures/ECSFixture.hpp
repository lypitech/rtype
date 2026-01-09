#pragma once

#include "../../fixtures/ComponentFixture.hpp"
#include "rtecs/ECS.hpp"

namespace rtecs::tests::fixture {

/**
 * @brief This fixture offers and ECS with pre-registered components, pre-registered entities and pre-registered systems :
 * -@code Components @endcode: Profile, Hitbox, Health
 * -@code Entities @endcode:
 *   - 0:@code Profile & Hitbox @endcode
 *   - 1:@code Profile & Hitbox @endcode
 *   - 2:@code Profile & Hitbox @endcode
 *   - 3:@code Hitbox & Health @endcode
 *   - 4:@code Hitbox & Health @endcode
 *   - 5:@code Hitbox & Health @endcode
 *   - 6:@code Health & Profile @endcode
 *   - 7:@code Health & Profile @endcode
 *   - 8:@code Health & Profile @endcode
 * -@code Systems @endcode:
 *   -@code RenameOnLowLifeSystem @endcode:@code Health & Profile @endcode
 *   -@code DamageOnCollideSystem @endcode:@code Hitbox & Health @endcode
 *   -@code LogOnCollideSystem @endcode:@code Profile & Hitbox @endcode
 *   -@code MoveToCenterSystem @endcode:@code Hitbox @endcode
 */
class ECSFixture : public ComponentFixture
{
    /**
     * @brief This system rename entities' profile on low life (health < 5).
     */
    class RenameOnLowLifeSystem final : public systems::ASystem
    {
    public:
        explicit RenameOnLowLifeSystem(const ECS& ecs);
        void apply(ECS& ecs) override;
    };

    /**
     * @brief This system reduce entities' health when they collide with another entity.
     */
    class DamageOnCollideSystem final : public systems::ASystem
    {
    public:
        explicit DamageOnCollideSystem(const ECS& ecs);
        void apply(ECS& ecs) override;
    };

    /**
     * @brief This system logs entities' name when they collide with another entity.
     */
    class LogOnCollideSystem final : public systems::ASystem
    {
    public:
        explicit LogOnCollideSystem(const ECS& ecs);
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
        explicit MoveToCenterSystem(const ECS& ecs);
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
