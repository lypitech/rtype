#pragma once

#include <memory>
#include <vector>

#include "behaviour.hpp"
#include "monoBehaviour.hpp"
#include "rtecs/ECS.hpp"

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
        _ecs->registerComponents<Components...>();
    }

    /**
     * @brief Registers a new entity into the @code ecs@endcode.
     * @tparam Components The list of components to create the entity with.
     * @param mono_behaviour A @code std::shared_ptr<behaviour>@endcode to add to this entity.
     * @param components The comma separated list of bracket initializers for the components.
     * @return The id of the newly created entity.
     */
    template <typename... Components>
    rtecs::types::EntityID registerEntity(
        const std::shared_ptr<behaviour::MonoBehaviour>& mono_behaviour,
        Components&&... components)
    {
        const rtecs::types::EntityID entityId = _ecs->registerEntity<std::decay_t<Components>...>(
            std::forward<Components>(components)...);

        if (!mono_behaviour) {
            return entityId;
        }
        _ecs->addEntityComponents<components::Behaviour>(entityId, {});
        rtecs::sparse::SparseGroup<components::Behaviour> behaviourGroup =
            _ecs->group<components::Behaviour>();

        behaviourGroup.apply([&](const rtecs::types::EntityID&, components::Behaviour& component) {
            component.instance = mono_behaviour;
            component.started = false;
        });
        components::Behaviour behaviourComp;
        behaviourComp.instance = mono_behaviour;
        behaviourComp.started = false;

        return entityId;
    }

    /**
     * @brief Runs one round of the game loop and apply all registered systems.
     * @param dt The elapsed time since the last call to this function in second.
     */
    void runOnce(double dt) const;

    /**
     * @brief Removes an entity from the ecs.
     * @param id The id of the entity to destroy.
     */
    void destroyEntity(const rtecs::types::EntityID& id) const;

    /**
     * @brief Updates the selected entity's specified components.
     * @tparam Comps The list of components to update.
     * @param id The id of the entity to update.
     * @param components The value of to update the components to.
     */
    template <typename... Comps>
    void updateEntity(const rtecs::types::EntityID& id,
                      Comps&&... components) const
    {
        _ecs->updateEntity<std::decay_t<Comps>...>(id, std::forward<Comps>(components)...);
    }

    /**
     * @brief Get a reference to the stored ecs.
     * @return A reference to the stored @code std::unique_ptr<<ECS>@endcode.
     */
    std::unique_ptr<rtecs::ECS>& getEcs() { return _ecs; }

private:
    std::unique_ptr<rtecs::ECS> _ecs;
};

}  // namespace rteng
