#include "rtecs/ECS.hpp"

#include "rtecs/systems/ISystem.hpp"
#include "rtecs/systems/SystemWrapper.hpp"

using namespace rtecs;

ECS::ECS()
    : _componentMaskIndex({std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000000}})
{
}

void ECS::registerSystem(std::unique_ptr<systems::ISystem>&& system) { _systems.push_back(std::move(system)); }

void ECS::registerSystem(std::function<void(ECS& ecs)> applyFn, const std::string& name = "UnknowSystem")
{
    registerSystem(std::make_unique<systems::SystemWrapper>(applyFn, name));
}

const bitset::DynamicBitSet& ECS::getEntityMask(const types::EntityID entityId) const
{
    if (!_entities.contains(entityId)) {
        return _emptyComponentMask;
    }
    return _entities.at(entityId);
}

void ECS::destroyEntity(types::EntityID entityId) const
{
    for (auto& _component : _components) {
        if (_component.second->has(entityId)) {
            _component.second->remove(entityId);
        }
    }
}

void ECS::applyAllSystems()
{
    for (const auto& system : _systems) {
        system->apply(*this);
    }
}
