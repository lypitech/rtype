#pragma once

#include "ASystem.hpp"
#include "ECS.hpp"
#include "comp/position.hpp"
#include "comp/rect.hpp"

namespace sys {

class IO : public rtecs::ASystem
{
public:
    explicit IO(const std::unique_ptr<rtecs::ECS>& ecs)
        : ASystem(ecs->getComponentsBitSet<comp::Position /*, comp::Velocity*/>())
    {
    }

    void apply(rtecs::ECS& ecs) override;
};

}  // namespace sys
