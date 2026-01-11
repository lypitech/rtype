#pragma once

#include <functional>
#include <vector>

#include "Transform.hpp"
#include "position.hpp"
#include "rect.hpp"
#include "rteng.hpp"
#include "rtnt/core/packet.hpp"
#include "type.hpp"
#include "health.hpp"
#include "collision.hpp"
#include "velocity.hpp"
#include "damage.hpp"
#include "invulnerability.hpp"
#include "owner.hpp"

namespace components {

using GameComponents = rteng::ComponentsList<
    Position,
    Transform,
    Rectangle,
    Type,
    Health,
    Collision,
    Velocity,
    Damage,
    Invulnerability,
    Owner
>;

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

using EntityInfos = std::pair<std::vector<uint8_t>, std::vector<uint8_t>>;

template <typename... Components>
EntityInfos getEntityComponentsInfos(rteng::ComponentsList<Components...>,
                                     rtecs::ECS& ecs,
                                     rtecs::EntityID id)
{
    size_t componentIndex = 0;
    const rtecs::Entity& entity = ecs.getEntities()[id];
    rtnt::core::Packet contentStream(0);

    auto packIfPresent = [&]<typename T>(T*) {
        if (entity[componentIndex]) {
            auto& sparseSet = dynamic_cast<rtecs::SparseSet<T>&>(ecs.getComponent<T>());

            if (auto val = sparseSet.get(id)) {
                contentStream << val.value().get();
            }
        }
        componentIndex++;
    };
    (packIfPresent(static_cast<Components*>(nullptr)), ...);
    return {entity.toBytes().first, contentStream.getPayload()};
}

}  // namespace components
