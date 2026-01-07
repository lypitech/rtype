#pragma once

#include <memory>
#include <typeindex>

#include "exceptions/UnknowComponentException.hpp"
#include "exceptions/ComponentAlreadyExists.hpp"
#include "logger/Logger.h"
#include "sparse/set/SparseSet.hpp"
#include "sparse/view/SparseView.hpp"
#include "rtecs/types/types.hpp"
#include "sparse/group/SparseGroup.hpp"
#include "systems/ASystem.hpp"

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
    std::vector<std::unique_ptr<systems::ASystem>> _systems;
    size_t _entitiesID = 0;

    /// Key: Component hashcode - Value: Pointer to an ISparseSet
    std::unordered_map<types::ComponentID, std::unique_ptr<sparse::ISparseSet>> _components;
    std::unordered_map<types::ComponentID, bitset::DynamicBitSet> _componentsMasks;
    bitset::DynamicBitSet _componentMaskIndex;

private:
    /**
     * @brief Register a single component.
     *
     * @throw exceptions::ComponentAlreadyExists If the component already exists.
     */
    template <typename T>
    void registerComponent()
    {
        if ((_componentMaskIndex >> 1).none())
            _componentMaskIndex.increase(1);
        _componentMaskIndex >>= 1;

        bitset::DynamicBitSet mask(_componentMaskIndex);
        types::ComponentID componentId = typeid(T).hash_code();
        auto sparse = std::make_unique<sparse::SparseSet<T>>(componentId);

        if (_components.contains(componentId))
            throw exceptions::ComponentAlreadyExists(componentId);

        _componentsMasks.emplace(componentId, mask);
        _components.emplace(componentId, std::move(sparse));
    }

    /**
     * @brief Add a component to an entity
     *
     * @throw exceptions::UnknowComponentException If the component is not registered.
     *
     * @tparam T The component type.
     * @param entity The entity.
     * @param instance The instance of the component that belong to the entity.
     */
    template <typename T>
    void insertComponentInstance(types::EntityID entity, T instance)
    {
        const types::ComponentID componentId = typeid(T).hash_code();

        if (!_components.contains(componentId))
            throw exceptions::UnknowComponentException(componentId);

        auto ptr = dynamic_cast<sparse::SparseSet<T>*>(_components.at(componentId).get());
        if (!ptr)
            throw exceptions::UnknowComponentException(componentId);
        ptr->put(entity, instance);
    }

    /**
     * @brief Get a component's mask from its type.
     *
     * @throw exceptions::UnknowComponentException If the component has not been registered.
     *
     * @tparam T The component type
     * @return The component's mask from its type.
     */
    template <typename T>
    const bitset::DynamicBitSet &getComponentMask() const
    {
        const types::ComponentID id = typeid(T).hash_code();

        if (!_componentsMasks.contains(id))
            throw exceptions::UnknowComponentException(id);
        return _componentsMasks.at(id);
    }

    /**
     * @brief Get the component set that contains the instances.
     *
     * @tparam T The component type
     * @return The component set.
     */
    template <typename T>
    sparse::SparseSet<T> &getComponent()
    {
        const types::ComponentID id = typeid(T).hash_code();

        if (!_components.contains(id))
            throw exceptions::UnknowComponentException(id);
        return _components.at(id);
    }

public:
    ECS() = default;
    ~ECS() = default;

    /****************/
    /**  ENTITIES  **/
    /****************/

    /**
     * @brief Register a new entity with its components.
     *
     * @tparam T The components of the entity.
     * @param instances The copies of the entity's components instances.
     * @return The new entity ID.
     */
    template<typename ...T>
    types::EntityID registerEntity(T ...instances)
    {
        bitset::DynamicBitSet mask;
        const types::EntityID entityId = _entitiesID;

        mask &= getComponentMask<T...>();
        addEntityComponent<T...>(entityId, instances...);
        _entities.insert({ entityId, mask });
        _entitiesID++;
        return entityId;
    }

    /**
     * @brief Add new components to an entity.
     *
     * @throw exceptions::UnknowComponentException If a component has not been registered.
     *
     * @tparam T The components' type to add to the entity.
     * @param entity The entity ID.
     * @param instances The copies of the entity's components instances.
     */
    template <typename ...T>
    void addEntityComponent(types::EntityID entity, T ...instances)
    {
        (insertComponentInstance(entity, instances), ...);
    }

      /******************/
     /**  COMPONENTS  **/
    /******************/

    /**
     * @brief Register new components to the ECS.
     *
     * @throw exceptions::ComponentAlreadyExists If a component already exists.
     * @tparam T The components' type to register.
     */
    template<typename ...T>
    void registerComponents()
    {
        (registerComponent<T>(), ...);
    }

    /**
     * @brief Get the component id.
     *
     * @throw exceptions::UnknowComponentException If the component has not been registered.
     * @tparam T The component type.
     * @return The id of the component.
     */
    template <typename T>
    types::ComponentID getComponentID() const
    {
        const types::ComponentID id = typeid(T).hash_code();

        if (!_components.contains(id))
            throw exceptions::UnknowComponentException(id);
        return id;
    }

    /**
     * @brief Get the mask corresponding to multiple components.
     *
     * @throw exceptions::UnknowComponentException If a component does not exist.
     *
     * @tparam T The components type
     * @return The mask that correspond to the components.
     */
    template <typename ...T>
    bitset::DynamicBitSet getMask() const
    {
        return (getComponentMask<T>() | ...);
    }

    template <typename ...T>
    const sparse::SparseView<T...> &view()
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
    void registerSystem(std::unique_ptr<systems::ASystem> &&system);

    /**
     * @brief Apply all the systems from the first registered to the last.
     */
    void applySystems();
};

}  // namespace rtecs
