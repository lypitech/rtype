#pragma once

#include <memory>

#include "ASystem.hpp"

namespace systems {

class Interpolation final : public rtecs::ASystem
{
public:
    Interpolation(const std::unique_ptr<rtecs::ECS>& ecs);

    void apply(rtecs::ECS& entity) override;
};

}  // namespace systems
