#pragma once
#include "ASystem.hpp"
#include "ECS.hpp"
#include "comp/position.hpp"
#include "comp/rect.hpp"

namespace sys {

class Rectangle : public rtecs::ASystem
{
public:
    explicit Rectangle(rtecs::DynamicBitSet bitset)
        : ASystem(std::move(bitset))
    {
    }
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace sys
