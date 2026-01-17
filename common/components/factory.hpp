#pragma once

#include <functional>
#include <vector>

#include "Transform.hpp"
#include "collision.hpp"
#include "health.hpp"
#include "invulnerability.hpp"
#include "owner.hpp"
#include "position.hpp"
#include "rteng.hpp"
#include "rtnt/core/packet.hpp"
#include "state.hpp"
#include "type.hpp"
#include "velocity.hpp"

namespace components {

using GameComponents = rteng::ComponentsList<Position,
                                             Transform,
                                             Type,
                                             Health,
                                             Collision,
                                             Velocity,
                                             Invulnerability,
                                             Owner,
                                             State>;

class Factory
{
public:
    using ComponentCreator =
        std::function<void(rtecs::ECS&, rtecs::types::EntityID, rtnt::core::Packet&)>;

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
               rtecs::bitset::DynamicBitSet bitmask,
               const std::vector<uint8_t>& data) const
    {
        LOG_TRACE_R2("Now reassembling entity {}", entityId);
        rtnt::core::Packet reader(data);

        for (size_t i = 0; i < _creators.size(); ++i) {
            if (bitmask[i + 1]) {
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

            ecs.addEntityComponents<T>(entityId, component);
        });
    }
};

}  // namespace components
