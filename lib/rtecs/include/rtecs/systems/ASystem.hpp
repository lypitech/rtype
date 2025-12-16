#pragma once

#include <utility>

#include "../bitset/DynamicBitSet.hpp"

namespace rtecs::system {

class ECS;  // Forward declaration for ECS type

class ASystem
{
private:
   bitset::DynamicBitSet _mask;

protected:
    [[nodiscard]]
    const bitset::DynamicBitSet &getMask() const noexcept
    {
        return _mask;
    };

public:
    explicit ASystem(bitset::DynamicBitSet mask)
        : _mask(std::move(mask)) {};
    virtual ~ASystem() = default;

    virtual void apply(ECS &ecs) = 0;
};

}  // namespace rtecs
