#pragma once

#include <memory>
#include <typeindex>

#include "logger/Logger.h"
#include "rtecs/types/types.hpp"
#include "sparse/group/SparseGroup.hpp"
#include "sparse/set/SparseSet.hpp"
#include "sparse/view/SparseView.hpp"
#include "systems/ISystem.hpp"

namespace rtecs {

/**
 * @brief This class is an ECS manager.
 *
 * The features of this ECS are :
 * - Register components
 * - Register systems
 * - Apply all registered systems on all entities
 * - Register entities
 * - Add a component to an entity
 * - Update multiple components of an entity
 * - Get specifics components of a single entity
 * - Get all the instances of a specific group (Transform + Gravity + Collidable) of all entities
 * - Delete an entity
 *
 * @note You can also instantiate an ECS using the ECS::createWithComponents<Your, Components, Here>();
 */
class ECS final
{
private:
    std::unordered_map<types::EntityID, types::Entity> _entities;
    std::vector<std::unique_ptr<systems::ISystem>> _systems;
    size_t _entitiesID = 0;

    /// Key: Component hashcode - Value: Pointer to an ISparseSet
    std::unordered_map<types::ComponentID, std::unique_ptr<sparse::ISparseSet>> _components;
    std::unordered_map<types::ComponentID, bitset::DynamicBitSet> _componentsMasks;
    bitset::DynamicBitSet _componentMaskIndex;
    bitset::DynamicBitSet _emptyComponentMask;

private:
    /**
     * @brief Register a single component.
     *
     * @warning If the component has already been registered, a warning will be logged but this will not impact the flow of the program.
     */
    template <typename T>
    void registerComponent()
    {
        if ((_componentMaskIndex >> 1).none()) {
            LOG_TRACE_R1("Increasing component mask index of 64 bits.");
            _componentMaskIndex.increase(1);
        }
        _componentMaskIndex >>= 1;

        bitset::DynamicBitSet mask(_componentMaskIndex);
        types::ComponentID componentId = typeid(T).hash_code();
        std::unique_ptr<sparse::SparseSet<T>> sparse =
            std::make_unique<sparse::SparseSet<T>>(componentId);

        if (_components.contains(componentId)) {
            LOG_WARN(
                "Cannot register the component#{}: This component has already been "
                "registered.",
                componentId);
            return;
        }
        _componentsMasks.emplace(componentId, mask);
        _components.emplace(componentId, std::move(sparse));
        LOG_TRACE_R2("Registered component#{} (\"{}\") with the following mask:\n[{}]",
                     componentId,
                     typeid(T).name(),
                     mask.toString().data());
    }

    /**
     * @brief Add a component to an entity
     *
     * @warning If the component has not been registered, a warning will be logged but this will not impact the flow of the program.
     *
     * @tparam T The component type.
     * @param entityId The entity.
     * @param instance The instance of the component that belong to the entity.
     */
    template <typename T>
    void insertComponentInstance(types::EntityID entityId,
                                 T instance)
    {
        const types::ComponentID componentId = typeid(T).hash_code();

        if (!_entities.contains(entityId)) {
            LOG_WARN("Cannot add the component {} to the entity {}: This entity does not exist.",
                     componentId,
                     entityId);
            return;
        }

        if (!_components.contains(componentId)) {
            LOG_WARN(
                "Cannot add the component {} to the entity {}: This component is not registered.",
                componentId,
                entityId);
            return;
        }

        auto ptr = dynamic_cast<sparse::SparseSet<T> *>(_components.at(componentId).get());
        if (!ptr) {
            LOG_WARN(
                "Cannot add the component {} to the entity {}: This component is not registered.",
                componentId,
                entityId);
            return;
        }
        _entities.at(entityId) |= getComponentMask<T>();
        LOG_TRACE_R3("Updated mask of entity#{}", entityId);
        ptr->put(entityId, instance);
        LOG_TRACE_R3("Updated component#{} of entity#{}", componentId, entityId);
    }

    /**
     * @brief Get a component's mask from its type.
     *
     * @warning If the component has not been registered, a warning will be logged and an empty mask will be returned.
     *
     * @tparam T The component type
     * @return The component's mask if the component has been registered, or an empty mask otherwise.
     */
    template <typename T>
    const bitset::DynamicBitSet &getComponentMaskHelper() const
    {
        const types::ComponentID id = typeid(T).hash_code();

        if (!_componentsMasks.contains(id)) {
            LOG_WARN("Cannot get the component#{}: This component has not been registered.", id);
            return _emptyComponentMask;
        }
        return _componentsMasks.at(id);
    }

    /**
     * @brief Get the component set that contains the instances.
     *
     * @warning If the component has not been registered, a warning will be logged and a `std::nullopt` will be returned.
     *
     * @tparam T The component type
     * @return An optional reference of the component set.
     */
    template <typename T>
    types::OptionalRef<sparse::SparseSet<T>> getComponent()
    {
        const types::ComponentID id = typeid(T).hash_code();

        if (!_components.contains(id)) {
            LOG_WARN("Cannot get the component#{}: This component does not exist.", id);
            return std::nullopt;
        }
        return dynamic_cast<sparse::SparseSet<T> &>(*_components.at(id));
    }

    /**
     * @brief Update a specific component of an entity.
     *
     * @warning If the component has not been registered, a warning will be logged and `false` will be returned.
     * @warning If the entity do not have this component, a warning will be logged and `false` will be returned.
     *
     * @tparam T The component type
     * @param entityId The entity's id
     * @param newInstance The new instance of the component
     * @return `true` if the entity has been updated, `false` otherwise.
     */
    template <typename T>
    bool updateEntityComponent(types::EntityID entityId,
                               T newInstance)
    {
        types::OptionalRef<sparse::SparseSet<T>> optSet = getComponent<T>();

        if (!optSet.has_value()) {
            LOG_WARN(
                "Cannot update component#{} of the entity#{}: This component has not been "
                "registered.",
                getComponentID<T>(),
                entityId);
            return false;
        }

        sparse::SparseSet<T> &set = optSet.value().get();

        if (!optSet.value().get().has(entityId)) {
            LOG_WARN(
                "Cannot update component#{} of the entity#{}: The entity do not have this "
                "component.",
                set.getId(),
                entityId);
            return false;
        }
        set.put(entityId, newInstance);
        LOG_TRACE_R3("Updated component#{} of entity#{}", set.getId(), entityId);
        return true;
    }

    /**
     * @brief Get the component id.
     *
     * @tparam T The component type.
     * @return The id of the component.
     */
    template <typename T>
    static types::ComponentID getComponentID()
    {
        return typeid(T).hash_code();
    }

public:
    explicit ECS();
    ~ECS() = default;

    /****************/
    /**  ENTITIES  **/
    /****************/

    /**
     * @brief Register a new entity with its components.
     *
     * @warning If none of the components has been registered, a warning will be logged and `types::NullEntityID` will be returned.
     *
     * @tparam T The components of the entity.
     * @param instances The copies of the entity's components instances.
     * @return The new entity ID if at least one of the components has been registered, `types::NullEntityID` otherwise.
     */
    template <typename... T>
    types::EntityID registerEntity(T... instances)
    {
        const types::EntityID entityId = _entitiesID;
        bitset::DynamicBitSet mask = (getComponentMask<T>() | ...);

        if (mask.none()) {
            LOG_CRIT("Cannot register entity with unregistered components. Component mask: {}",
                     mask.toString(" "));
            return types::NullEntityID;
        }
        _entities.insert({entityId, mask});
        LOG_TRACE_R2("Entity#{} registered.", entityId);
        addEntityComponents<T...>(entityId, instances...);
        _entitiesID++;
        return entityId;
    }

    /**
     * @brief Pre-register an empty entity.
     * @return The new entity ID.
     */
    types::EntityID preRegisterEntity();

    /**
     * @brief Add new components to an entity.
     *
     * @warning If one of the components has not been registered, a warning will be logged but this will not impact the flow of the program.
     *
     * @tparam T The components' type to add to the entity.
     * @param entity The entity ID.
     * @param instances The copies of the entity's components instances.
     */
    template <typename... T>
    void addEntityComponents(types::EntityID entity,
                             T... instances)
    {
        (insertComponentInstance(entity, instances), ...);
    }

    /**
     * @brief Update multiple components instances of an entity.
     *
     * @warning If one of the components has not been registered, a warning will be logged and `false` will be returned.
     * @warning If the entity do not have one of the components, a warning will be logged and `false` will be returned.
     *
     * @tparam Ts The components type
     * @param entityId The entity's id
     * @param newInstances The new instances of the components.
     * @return `false` if at least one instance has not been updated, `true` otherwise.
     */
    template <typename... Ts>
    bool updateEntity(const types::EntityID entityId,
                      Ts... newInstances)
    {
        return (... && updateEntityComponent<Ts>(entityId, newInstances));
    }

    /**
     * @brief Get the mask of an entity.
     *
     * @param entityId The entity's ID.
     * @return The mask of the entity or an empty mask if the entity has not been registered.
     */
    const bitset::DynamicBitSet &getEntityMask(types::EntityID entityId) const;

    /**
     * @brief Remove an entity from the ECS.
     *
     * @param entityId The entity's ID
     */
    void destroyEntity(types::EntityID entityId);

    /******************/
    /**  COMPONENTS  **/
    /******************/

    /**
     * @brief Register new components to the ECS.
     *
     * @warning If one of the components has already been registered, a warning will be logged but this will not impact the flow of the program.
     *
     * @tparam T The components' type to register.
     */
    template <typename... T>
    void registerComponents()
    {
        (registerComponent<T>(), ...);
    }

    /**
     * @brief Get the mask corresponding to multiple components.
     *
     * @warning Each of the component that have not been registered will not appear in the mask. A warning will be logged for each of those components.
     *
     * @tparam T The components type
     * @return The mask that correspond to the components.
     */
    template <typename... T>
    bitset::DynamicBitSet getComponentMask() const
    {
        return (getComponentMaskHelper<T>() | ...);
    }

    /**
     * @brief Group all entities that have at least all the specified components.
     *
     * @tparam T The components to store in the group
     * @return The corresponding SparseGroup.
     */
    template <typename... T>
    sparse::SparseGroup<T...> group()
    {
        return sparse::SparseGroup<T...>(getComponent<T>()...);
    }

    /***************/
    /**  SYSTEMS  **/
    /***************/

    /**
     * @brief Register and move the system instance.
     *
     * @warning This method moves the system instance. After this call, your unique pointer will no longer be accessible.
     *
     * @param system A unique pointer that will be moved to the ECS.
     */
    void registerSystem(std::unique_ptr<systems::ISystem> &&system);

    /**
     * @brief Create a new system class from the SystemWrapper.
     *
     * @warning This method creates a system class from the given apply method.
     *
     * @param applyFn A function that correspond to the apply method of the System.
     * @param name The name of the registered system.
     */
    void registerSystem(const std::function<void(ECS &ecs)> &applyFn,
                        const std::string &name);

    /**
     * @brief Apply all the systems from the first registered to the last.
     */
    void applyAllSystems();
};

}  // namespace rtecs
