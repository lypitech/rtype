#pragma once

#include <functional>
#include <vector>

#include "Transform.hpp"
#include "collision.hpp"
#include "damage.hpp"
#include "health.hpp"
#include "hitbox.hpp"
#include "invulnerability.hpp"
#include "owner.hpp"
#include "position.hpp"
#include "rteng.hpp"
#include "rtnt/core/packet.hpp"
#include "score.hpp"
#include "state.hpp"
#include "type.hpp"
#include "value.hpp"
#include "velocity.hpp"

namespace components {

using GameComponents = rteng::ComponentsList<Collision,
                                             Damage,
                                             Health,
                                             Hitbox,
                                             Invulnerability,
                                             Owner,
                                             Position,
                                             Score,
                                             State,
                                             Transform,
                                             Type,
                                             Value,
                                             Velocity>;

static const std::unordered_map<entity::Type, Hitbox> entityTypeToHitbox = {
    {entity::Type::kEnemy, {true, 150, 75}}};

static const std::unordered_map<entity::Type, Velocity> entityTypeToVelocity = {
    {entity::Type::kEnemy, {0, 0, 15, 15}}};

static const std::unordered_map<entity::Type, Value> entityTypeToValue = {
    {entity::Type::kEnemy, {15}}};

inline Hitbox typeToHitbox(const entity::Type& type) { return entityTypeToHitbox.at(type); }

inline Velocity typeToVelocity(const entity::Type& type) { return entityTypeToVelocity.at(type); }

inline Value typeToValue(const entity::Type& type) { return entityTypeToValue.at(type); }

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
