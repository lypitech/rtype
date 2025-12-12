#pragma once

#include <memory>

#include "behaviour/MonoBehaviour.hpp"

namespace comp {

struct Behaviour
{
    std::shared_ptr<rteng::behaviour::MonoBehaviour> instance;
    bool started = false;
};

}  // namespace comp
