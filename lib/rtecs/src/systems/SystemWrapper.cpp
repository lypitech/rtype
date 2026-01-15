#include "rtecs/systems/SystemWrapper.hpp"

#include <iostream>

rtecs::systems::SystemWrapper::SystemWrapper(const std::function<void(ECS&)>& applyFn,
                                             const std::string& name = "UnknowSystem")
    : ASystem(name),
      _applyFn(applyFn)
{
}

void rtecs::systems::SystemWrapper::apply(ECS& ecs) { _applyFn(ecs); }
