#pragma once
#include <string>
#include <utility>

#include "ASystem.hpp"
#include "ECS.hpp"
#include "comp/Sprite.hpp"
#include "comp/position.hpp"

namespace sys {

class Sprite : public rtecs::ASystem
{
public:
    explicit Sprite(const std::unique_ptr<rtecs::ECS>& ecs)
        : ASystem(ecs->getComponentsBitSet<comp::Sprite,
                                           comp::Position>())
    {
    }
    void apply(rtecs::ECS& ecs) override;
};

class Animation : public rtecs::ASystem
{
public:
    explicit Animation(const std::unique_ptr<rtecs::ECS>& ecs)
        : ASystem(ecs->getComponentsBitSet<comp::Sprite>())
    {
    }
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace sys
