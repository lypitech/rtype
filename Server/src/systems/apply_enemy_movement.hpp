#pragma once

#include "rtecs/systems/ASystem.hpp"

namespace server::systems {

class ApplyEnemyMovement final : public rtecs::systems::ASystem
{
public:
    explicit ApplyEnemyMovement();
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace server::systems
