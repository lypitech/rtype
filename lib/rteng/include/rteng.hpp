#pragma once

#include <memory>
#include <vector>

#include "Behaviour.hpp"
#include "ECS.hpp"
#include "MonoBehaviour.hpp"

namespace rteng {

template <typename... Ts>
struct ComponentsList
{
};

/**
 * @class GameEngine
 * @brief A class used to wrap and run an ecs.
 */
class GameEngine
{
public:
    /**
     * @brief Creates a @code GameEngine@endcode containing an @code ecs@endcode.
     * @tparam Components The list of components to create the @code ecs@endcode with.
     */
    template <typename... Components>
    explicit GameEngine(ComponentsList<Components...>)
    {
        _ecs = rtecs::ECS::createWithComponents<Components..., components::Behaviour>();
    }

    /**
     * @brief Registers a new entity into the @code ecs@endcode.
     * @tparam Components The list of components to create the entity with.
     * @param mono_behaviour A @code std::shared_ptr<behaviour>@endcode to add to this entity.
     * @param components The comma separated list of bracket initializers for the components.
     * @return The id of the newly created entity.
     */
    template <typename... Components>
    rtecs::EntityID registerEntity(const std::shared_ptr<behaviour::MonoBehaviour>& mono_behaviour,
                                   Components&&... components)
    {
        const rtecs::EntityID entityId = _ecs->registerEntity<std::decay_t<Components>...>(
            std::forward<Components>(components)...);

        if (!mono_behaviour || !_ecs->hasEntityComponent<components::Behaviour>(entityId)) {
            return entityId;
        }
        auto& behaviourComponents = _ecs->getComponent<components::Behaviour>();
        auto& behaviourSparseSet =
            dynamic_cast<rtecs::SparseSet<components::Behaviour>&>(behaviourComponents);

        components::Behaviour behaviourComp;
        behaviourComp.instance = mono_behaviour;
        behaviourComp.started = false;

        behaviourSparseSet.put(entityId, behaviourComp);
        return entityId;
    }

    /**
     * @brief Runs one round of the game loop and apply all registered systems.
     * @param dt The elapsed time since the last call to this function in second.
     */
    void runOnce(double dt) const;

    /**
     * @brief Get a reference to the stored ecs.
     * @return A reference to the stored @code std::unique_ptr<<ECS>@endcode.
     */
    std::unique_ptr<rtecs::ECS>& getEcs() { return _ecs; }

private:
    std::unique_ptr<rtecs::ECS> _ecs;
};

}  // namespace rteng
