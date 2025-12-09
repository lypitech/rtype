#include "ECS.hpp"

#include "ASystem.hpp"

using namespace rtecs;

void ECS::registerSystem(const SystemID id, std::unique_ptr<ASystem> system) { _systemView[id] = std::move(system); }

void ECS::applySystem(const SystemID id) { _systemView[id]->apply(*this); }

void ECS::applyAllSystems()
{
    for (const auto &system : _systemView.getDense()) {
        system->apply(*this);
    }
}
