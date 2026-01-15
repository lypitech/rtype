#include "ECS.hpp"

#include "ASystem.hpp"

using namespace rtecs;

ECS::ECS()
    : _componentMaskIndex(
          {std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000000}})
{
    LOG_TRACE_R2("ECS created.");
}

void ECS::registerSystem(std::unique_ptr<systems::ISystem>&& system)
{
    LOG_TRACE_R2("Registered system \"{}\"", system->getName().data());
    _systems.push_back(std::move(system));
}

void ECS::registerSystem(const std::function<void(ECS& ecs)>& applyFn,
                         const std::string& name = "UnknowSystem")
{
    LOG_TRACE_R2("Registered lambda system \"{}\"", name.data());
    registerSystem(std::make_unique<systems::SystemWrapper>(applyFn, name));
}

const bitset::DynamicBitSet& ECS::getEntityMask(const types::EntityID entityId) const
{
    if (!_entities.contains(entityId)) {
        return _emptyComponentMask;
    }
    return _entities.at(entityId);
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
    for (auto&& system : _systems) {
        system->apply(*this);
    }
}
