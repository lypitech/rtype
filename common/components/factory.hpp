#pragma once

#include <functional>
#include <vector>

#include "Transform.hpp"
#include "position.hpp"
#include "rect.hpp"
#include "rteng.hpp"
#include "rtnt/core/packet.hpp"

namespace components {

using GameComponents = rteng::ComponentsList<Position, Transform, Rectangle>;

class Factory
{
public:
    using ComponentCreator = std::function<void(rtecs::ECS&, rtecs::EntityID, rtnt::core::Packet&)>;

    template <typename... Components>
    explicit Factory(rteng::ComponentsList<Components...>)
    {
        (registerComponent<Components>(), ...);
    }

    /**
     * @brief Populates the entity according to given bitmask
     */
    void apply(rtecs::ECS& ecs,
               const size_t entityId,
               rtecs::DynamicBitSet bitmask,
               const std::vector<uint8_t>& data) const
    {
        rtnt::core::Packet reader(data);

        for (size_t i = 0; i < _creators.size(); ++i) {
            if (bitmask[i]) {
                if (i < _creators.size()) {
                    _creators[i](ecs, entityId, reader);
                }
            }
        }
    }

private:
    std::vector<ComponentCreator> _creators;

    template <typename T>
    void registerComponent()
    {
        _creators.push_back([](rtecs::ECS& ecs, size_t entityId, rtnt::core::Packet& p) {
            LOG_DEBUG("Reassembling component ({})", typeid(T).name());
            T component;

            p >> component;

            auto& sparseSet = dynamic_cast<rtecs::SparseSet<T>&>(ecs.getComponent<T>());
            sparseSet.put(entityId, component);
        });
    }
};

}  // namespace components
