#pragma once

#include "rtecs/types.hpp"

namespace rtecs {

class ECS; // Forward declaration

class ASystem {
private:
    const BitMask _mask;
protected:
    [[nodiscard]]
    BitMask getMask() const noexcept { return _mask; };
public:
    explicit ASystem(const BitMask mask): _mask(mask) {};
    virtual ~ASystem() = default;
    virtual void apply(ECS &ecs) = 0;
};

}
