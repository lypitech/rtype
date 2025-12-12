#pragma once

#include <utility>

#include "DynamicBitSet.hpp"

namespace rtecs {

class ECS;  // Forward declaration for ECS type

class ASystem
{
   private:
    DynamicBitSet _mask;

   protected:
    [[nodiscard]]
    const DynamicBitSet &getMask() const noexcept
    {
        return _mask;
    };

public:
    explicit ASystem(DynamicBitSet mask)
        : _mask(std::move(mask)) {};
    virtual ~ASystem() = default;

    virtual void apply(ECS &ecs) = 0;
};

}  // namespace rtecs
