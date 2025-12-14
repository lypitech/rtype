#pragma once

#include <functional>
#include <vector>

#include "comp/Behaviour.hpp"
#include "comp/IO.hpp"
#include "comp/Sprite.hpp"
#include "comp/Transform.hpp"
#include "rtnt/Core/packet.hpp"

namespace rteng {

#define ALL_COMPONENTS comp::Behaviour, comp::Position, comp::Transform, comp::IO, comp::Sprite

class ComponentFactory
{
public:
    using ComponentCreator = std::function<void(rtecs::ECS&, rtecs::EntityID, rtnt::core::Packet&)>;

    ComponentFactory() { registerAll<ALL_COMPONENTS>(); }

    /**
     * @brief Populates the entity according to given bitmask
     */
    void apply(rtecs::ECS& ecs, size_t entityId, rtecs::DynamicBitSet bitmask, const std::vector<uint8_t>& data)
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
            LOG_DEBUG("Called for entityID: {} and type {}", entityId, typeid(T).name());
            T component;

            p >> component;

            auto& sparseSet = dynamic_cast<rtecs::SparseSet<T>&>(ecs.getComponent<T>());
            sparseSet.put(entityId, component);
        });
    }

    template <typename... Ts>
    void registerAll()
    {
        (registerComponent<Ts>(), ...);
    }
};

}  // namespace rteng
