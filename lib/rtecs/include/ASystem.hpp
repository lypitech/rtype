#pragma once

#include "ECS.hpp"

namespace rtecs {

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

    explicit ASystem()
        : _mask() {};

    template <typename... Components>
    explicit ASystem(const ECS &ecs)
        : _mask(ecs.getComponentsBitSet<Components...>()){};

   public:
    virtual ~ASystem() = default;

    virtual void apply(ECS &ecs) = 0;
};

}  // namespace rtecs
#include "rtecs/types.hpp"

namespace rtecs {

class ECS;  // Forward declaration

class ASystem {
   private:
    const BitMask _mask;

   protected:
    [[nodiscard]]
    BitMask getMask() const noexcept {
        return _mask;
    };

   public:
    explicit ASystem(const BitMask mask) : _mask(mask) {};
    virtual ~ASystem() = default;
    virtual void apply(ECS &ecs) = 0;
};

}  // namespace rtecs
