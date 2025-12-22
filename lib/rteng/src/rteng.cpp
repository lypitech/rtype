#include "rteng.hpp"

#include "SparseSet.hpp"
#include "comp/Behaviour.hpp"

namespace rteng {

template <typename... Components>
rtecs::EntityID GameEngine::registerEntity(
    const std::shared_ptr<behaviour::MonoBehaviour>& mono_behaviour,
    Components&&... components)
{
    const rtecs::EntityID entityId =
        _ecs->registerEntity<std::decay_t<Components>...>(std::forward<Components>(components)...);

    if (!mono_behaviour || !_ecs->hasEntityComponent<comp::Behaviour>(entityId)) {
        return entityId;
    }
    auto& behaviourComponents = _ecs->getComponent<comp::Behaviour>();
    auto& behaviourSparseSet =
        dynamic_cast<rtecs::SparseSet<comp::Behaviour>&>(behaviourComponents);

    comp::Behaviour behaviourComp;
    behaviourComp.instance = mono_behaviour;
    behaviourComp.started = false;

    behaviourSparseSet.put(entityId, behaviourComp);
    return entityId;
}

void GameEngine::runOnce(const double dt) const
{
    {
        auto& behaviourComponents = _ecs->getComponent<comp::Behaviour>();
        auto& behaviourSparseSet =
            dynamic_cast<rtecs::SparseSet<comp::Behaviour>&>(behaviourComponents);

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
