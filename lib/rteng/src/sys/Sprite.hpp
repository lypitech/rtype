#pragma once
#include <string>
#include <utility>

#include "ASystem.hpp"
#include "comp/Sprite.hpp"

namespace sys {

class Sprite : public rtecs::ASystem
{
public:
    explicit Sprite(rtecs::DynamicBitSet bitset)
        : ASystem(std::move(bitset))
    {
    }
    void apply(rtecs::ECS& ecs) override;  // equivalent of render.
};

class Animation : public rtecs::ASystem
{
public:
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace sys
