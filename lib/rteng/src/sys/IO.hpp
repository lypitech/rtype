#pragma once

#include "ASystem.hpp"

namespace sys {

class IO : public rtecs::ASystem
{
public:
    void apply(rtecs::ECS& ecs) override;
};

}  // namespace sys
