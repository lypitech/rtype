#include "rtecs/ECS.hpp"

#include "rtecs/systems/ASystem.hpp"

using namespace rtecs;

void ECS::registerSystem(std::unique_ptr<systems::ASystem>&& system) { _systems.push_back(std::move(system)); }

void ECS::applySystems()
{
    for (const auto& system : _systems) {
        system->apply(*this);
    }
}
