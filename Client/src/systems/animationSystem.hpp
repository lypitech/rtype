#pragma once

#include "rtecs/ECS.hpp"
#include "rtecs/systems/ASystem.hpp"

namespace systems {

class AnimationSystem final : public rtecs::systems::ASystem
{
public:
    AnimationSystem();
    ~AnimationSystem() override = default;

    void apply(rtecs::ECS& ecs) override;
};

}  // namespace systems
