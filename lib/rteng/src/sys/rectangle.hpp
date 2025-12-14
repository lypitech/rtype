#pragma once
#include "ASystem.hpp"
#include "ECS.hpp"
#include "comp/position.hpp"
#include "comp/rect.hpp"

namespace sys {

class Rectangle : public rtecs::ASystem
{
public:
    explicit Rectangle(const std::unique_ptr<rtecs::ECS>& ecs)
        : ASystem(ecs->getComponentsBitSet<comp::Rectangle, comp::Position>())
    {
    }
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace sys
