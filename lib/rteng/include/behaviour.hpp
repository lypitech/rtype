#pragma once

#include <memory>

#include "monoBehaviour.hpp"

namespace components {

struct Behaviour
{
    std::shared_ptr<rteng::behaviour::MonoBehaviour> instance;
    bool started = false;

    template <typename Archive>
    void serialize(Archive&)
    {
        // LALALALLALA J4ENTENDS PAS
    }
};

}  // namespace components
