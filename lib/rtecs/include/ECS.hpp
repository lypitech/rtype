#pragma once

#include <memory>

#include "ASystem.hpp"
#include "DynamicBitSet.hpp"
#include "ISparseSet.hpp"
#include "SparseSet.hpp"
#include "SparseVectorView.hpp"
#include "View.hpp"
#include "logger/Logger.h"

namespace rtecs {

using Entity = DynamicBitSet;
using EntityID = size_t;
using SystemID = size_t;
/**
 * @brief Type used to identify a component type in runtime maps.
 *
 * Uses `typeid(...).hash_code()` to produce a stable key for component
 * registration/lookups.
 */
using ComponentID = decltype(typeid(ISparseSet).hash_code());

/**
 * @brief Helper: a vector of strong references to a component type.
 *
 * Used as the element type when returning groups of component references
 * (for example `getMultipleComponents`).
 */
template <typename Component>
using ComponentGroup = std::vector<std::reference_wrapper<Component>>;

/**
 * @brief Tuple of `ComponentGroup`s for variadic component queries.
 */
template <typename... Components>
using ComponentGroupList = std::tuple<ComponentGroup<Components>...>;

/**
 * @brief Main ECS container.
 *
 * Responsibilities:
 * - Register component storage (`registerComponent<T>()`).
 * - Register systems (`registerSystem`).
 * - Create/register entities with an associated component mask and
 *   component instances (`registerEntity`).
 * - Query components by entity or by component group.
 *
 * Design notes:
 * - Entities are represented by an `Entity` (`DynamicBitSet`) mask and
 *   stored in `_entityList` where their `EntityID` is the index.
 * - Component storage is type-erased behind `ISparseSet` and kept in a
 *   `SparseVectorView` mapping `ComponentID` -> `ISparseSet`.
 * - Component lookups use `typeid(T).hash_code()` as the runtime key.
 */
class ECS final
{
private:
    /**
     * @brief Construct or assign a component instance for `id` inside the
     *        appropriate `SparseSet<Component>`.
     *
     * This forwards the provided component to the concrete `SparseSet`
     * instance stored in `_componentView`.
     */
    template <typename Component>
    void emplaceComponent(EntityID id, Component &&component)
    {
        const size_t hashcode = typeid(std::remove_reference_t<Component>).hash_code();

        auto &sparseSet = static_cast<SparseSet<std::remove_reference_t<Component>> &>(*_componentView[hashcode]);

        sparseSet.put(id, std::forward<Component>(component));
    }

    /**
     * @brief Build a `DynamicBitSet` with a single bit set for the given
     *        component type.
     *
     * The returned bitset has the bit corresponding to the dense index of
     * the component registration set to `true`. Used to compose entity
     * masks when registering entities or matching systems.
     */
    template <typename Component>
    DynamicBitSet getComponentBitSet() const
    {
        DynamicBitSet bitset;
        const size_t hashcode = typeid(Component).hash_code();

        bitset[_componentView.getDenseIndex(hashcode)] = true;
        return bitset;
    }

    void applySystem(SystemID id);

public:
    explicit ECS() = default;
    ECS(const ECS &) = delete;
    ECS &operator=(const ECS &) = delete;
    ECS(ECS &&) = default;
    ECS &operator=(ECS &&) = default;

    /**
     * @brief Creates a View to iterate over entities having all specified components.
     *
     *  Usage: @code for (auto [pos, vel] : ecs.view<Position, Velocity>()) { ... }@endcode
     *
     * @returns A @code View@endcode to iterate over said entities.
     *
     */
    template <typename... Components>
    View<Components...> view()
    {
        return View<Components...>(dynamic_cast<SparseSet<Components> &>(getComponent<Components>())...);
    }

    /**
     * @brief Convenience factory creating an `ECS` and registering the
     *        given component types.
     *
     * Example: `ECS::createWithComponents<Position, Velocity>()` will
     * register those component types and return an `ECS` instance.
     */
    template <typename... Components>
    static std::unique_ptr<ECS> createWithComponents()
    {
        std::unique_ptr<ECS> e = std::make_unique<ECS>();
        (e->registerComponent<Components>(), ...);
        return e;
    }

    ~ECS() = default;

    /**
     * @brief Build a combined component mask for the provided component
     *        types (bitwise OR of each component's bitset).
     */
    template <typename... Components>
    DynamicBitSet getComponentsBitSet() const
    {
        return (getComponentBitSet<Components>() | ...);
    }

    /**
     * @brief Get the list of all registered entity signatures.
     * Useful for iterating over all valid IDs (0 to size-1).
     */
    [[nodiscard]]
    const std::vector<Entity> &getEntities() const
    {
        return _entityList;
    }

    template <typename Component, typename... TupleTypes>
    static constexpr ComponentGroup<Component> &getComponentGroup(std::tuple<TupleTypes...> &tuple)
    {
        return std::get<ComponentGroup<Component>>(tuple);
    }

    /**
     * @brief Register component storage for `Component`.
     *
     * After calling this the ECS can store instances of `Component` and
     * return references via `getComponent` / `getComponentOf`.
     */
    template <typename Component>
    void registerComponent()
    {
        const size_t hashcode = typeid(Component).hash_code();
        _componentView.emplace(hashcode, std::make_unique<SparseSet<Component>>());
        LOG_DEBUG("Registered new component({})", typeid(Component).name());
    }

    /**
     * @brief Register a new entity and optionally emplace provided
     *        component instances.
     *
     * The returned `EntityID` is the entity's index in `_entityList`.
     */
    template <typename... Components>
    EntityID registerEntity(Components &&...components)
    {
        const DynamicBitSet entity = getComponentsBitSet<Components...>();

        _entityList.push_back(entity);

        const EntityID id = _entityList.size() - 1;

        LOG_DEBUG("Created entity.");
        (emplaceComponent(id, std::forward<Components>(components)), ...);
        return id;
    }

    /**
     * @brief Register a new entity with an empty instance set but a
     *        component mask reflecting the provided component types.
     */
    template <typename... Components>
    EntityID registerEntity()
    {
        const DynamicBitSet entity(getComponentsBitSet<Components...>());

        _entityList.push_back(entity);
        return _entityList.size() - 1;
    }

    /**
     * @brief Register a new entity with the provided component mask.
     */
    EntityID registerEntity(const DynamicBitSet &entity)
    {
        _entityList.push_back(entity);
        return _entityList.size() - 1;
    }

    /**
     * @brief Check whether `entityId` has `Component` (by mask test).
     */
    template <typename Component>
    bool hasEntityComponent(EntityID entityId)
    {
        const DynamicBitSet &entity = _entityList[entityId];
        const DynamicBitSet componentBitset = getComponentBitSet<Component>();

        return (entity & componentBitset) == componentBitset;
    }

    /**
     * @brief Register a system instance. Systems must derive from
     *        `ASystem`.
     */
    template <typename System>
    void registerSystem(std::unique_ptr<System> system)
    {
        static_assert(std::is_base_of_v<ASystem, System>, "System must inherit from ASystem");
        const size_t hashcode = typeid(System).hash_code();
        _systemView.emplace(hashcode, std::move(system));
        LOG_DEBUG("Registered new system({})", typeid(System).name());
    }

    /**
     * @brief Apply the system type `System` (dispatch by typeid).
     */
    template <typename System>
    void applySystem()
    {
        static_assert(std::is_base_of<ASystem, System>::value, "System must inherit from ASystem");
        const size_t hashcode = typeid(System).hash_code();
        applySystem(hashcode);
    }

    /** @brief Apply all registered systems. */
    void applyAllSystems();

    /**
     * @brief Return the `ISparseSet` backing storage for `Component`.
     *
     * Caller is responsible for casting to a concrete `SparseSet<T>` if
     * they need access to typed operations.
     */
    template <typename Component>
    ISparseSet &getComponent()
    {
        const size_t hashcode = typeid(Component).hash_code();
        return *_componentView[hashcode];
    };

    /**
     * @brief Collect and return groups of component references for
     *        entities matching the provided component types.
     *
     * Returns a tuple of vectors (one per component type) containing
     * references to each matched component instance.
     */
    template <typename... Components>
    ComponentGroupList<Components...> getMultipleComponents()
    {
        std::tuple<std::vector<Components>...> tuple{};
        DynamicBitSet bitset;

        ((bitset |= getComponentBitSet<Components>()), ...);
        for (const auto &entity : _entityList) {
            if ((entity & bitset) == bitset) {
                (std::get<std::vector<Components>>(tuple).push_back(getComponentOf<Components>(entity)), ...);
            }
        }
        return tuple;
    }

    /**
     * @brief Retrieve a component instance for `entity` by type.
     *
     * This performs a lookup in the concrete `SparseSet<Component>` and
     * returns a reference to the component. Behavior is undefined if the
     * entity does not actually own the component (caller must ensure
     * membership with `hasEntityComponent`).
     */
    template <typename Component>
    Component &getComponentOf(EntityID entity)
    {
        const ComponentID componentId = typeid(Component).hash_code();
        auto sparseSet = dynamic_cast<SparseSet<Component>>(_componentView.getDenseIndex(componentId));

        return sparseSet.get(entity);
    }

private:
    std::vector<Entity> _entityList;
    SparseVectorView<ComponentID, std::unique_ptr<ISparseSet>> _componentView;
    SparseVectorView<SystemID, std::unique_ptr<ASystem>> _systemView;
};

}  // namespace rtecs
