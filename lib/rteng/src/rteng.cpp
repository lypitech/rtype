#include "rteng.hpp"

#include "Behaviour.hpp"
#include "SparseSet.hpp"

namespace rteng {

void GameEngine::runOnce(const double dt) const
{
    {
        auto& behaviourComponents = _ecs->getComponent<components::Behaviour>();
        auto& behaviourSparseSet =
            dynamic_cast<rtecs::SparseSet<components::Behaviour>&>(behaviourComponents);

        for (auto& [instance, started] : behaviourSparseSet.getAll()) {
            if (!instance) {
                continue;
            }
            if (!started) {
                instance->Start();
                started = true;
            }
            instance->Update(dt);
        }
    }
    _ecs->applyAllSystems();
}

}  // namespace rteng
