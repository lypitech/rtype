#pragma once

#include <memory>

#include "rtecs/systems/ASystem.hpp"

namespace systems {

class Interpolation final : public rtecs::systems::ASystem
{
public:
    Interpolation();

    void apply(rtecs::ECS& entity) override;
};

}  // namespace systems
