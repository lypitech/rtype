#pragma once

#include "ASystem.hpp"

#include <memory>

namespace systems {

class Interpolation final : public rtecs::ASystem
{
public:
    Interpolation(const std::unique_ptr<rtecs::ECS>& ecs);

    void apply(rtecs::ECS& entity) override;
};

}  // namespace systems
