#include "rtecs/ECS.hpp"

#include "rtecs/systems/ISystem.hpp"
#include "rtecs/systems/SystemWrapper.hpp"

using namespace rtecs;

ECS::ECS()
    : _componentMaskIndex(
          {std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000000}})
{
    LOG_TRACE_R2("ECS created.");
}

types::EntityID ECS::preRegisterEntity()
{
    types::EntityID entityId = _entitiesID;

    _entitiesID++;
    _entities.insert({entityId, bitset::DynamicBitSet{}});
    LOG_TRACE_R2("Entity#{} pre-registered.", entityId);
    return entityId;
}

void ECS::registerSystem(const std::shared_ptr<systems::ISystem>& system)
{
    _systems.push_back(system);
}

void ECS::registerSystem(const std::function<void(ECS& ecs)>& applyFn,
                         const std::string& name = "UnknowSystem")
{
    registerSystem(std::make_shared<systems::SystemWrapper>(applyFn, name));
}

const bitset::DynamicBitSet& ECS::getEntityMask(const types::EntityID entityId) const
{
    if (!_entities.contains(entityId)) {
        return _emptyComponentMask;
    }
    return _entities.at(entityId);
}

std::vector<types::EntityID> ECS::getAllEntities() const
{
    std::vector<types::EntityID> entities;

    for (const auto& entity : _entities | std::views::keys) {
        entities.push_back(entity);
    }
    return entities;
}

void ECS::destroyEntity(const types::EntityID entityId)
{
    for (auto& _component : _components) {
        if (_component.second->has(entityId)) {
            LOG_TRACE_R2("Removing component {} from entity#{}", _component.first, entityId);
            _component.second->remove(entityId);
        }
    }
    _entities.erase(entityId);
    LOG_TRACE_R2("Destroyed entity#{}", entityId);
}

void ECS::applyAllSystems()
{
    for (const auto& system : _systems) {
        system->apply(*this);
    }
}
