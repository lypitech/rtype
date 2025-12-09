#pragma once

#include "DynamicBitSet.hpp"
#include "ISparseSet.hpp"
#include "SparseSet.hpp"
#include "SparseVectorView.hpp"

namespace rtecs {

class ASystem;  // Forward declaration for system type

using Entity = DynamicBitSet;
using EntityID = size_t;
using SystemID = size_t;
using ComponentID = decltype(typeid(ISparseSet).hash_code());

template <typename Component>
using ComponentGroup = std::vector<std::reference_wrapper<Component>>;

template <typename... Components>
using ComponentGroupList = std::tuple<ComponentGroup<Components>...>;

class ECS final
{
   private:
    template <typename Component>
    DynamicBitSet getComponentBitSet() const
    {
        DynamicBitSet bitset;
        const size_t hashcode = typeid(Component).hash_code();

        bitset[_componentView.getDenseIndex(hashcode)] = true;
        return bitset;
    }

    void applySystem(const SystemID id);

   public:
    explicit ECS() = default;

    template <typename... Components>
    explicit ECS()
    {
        (registerComponent<Components>(), ...);
    }

    ~ECS() = default;

    template <typename... Components>
    DynamicBitSet getComponentsBitSet() const
    {
        return (getComponentBitSet<Components>(), ...);
    }

    template <typename Component, typename... TupleTypes>
    static constexpr ComponentGroup<Component> &getComponentGroup(std::tuple<TupleTypes...> &tuple)
    {
        return std::get<ComponentGroup<Component>>(tuple);
    }

    template <typename Component>
    void registerComponent()
    {
        const size_t hashcode = typeid(Component).hash_code();
        _componentView[hashcode] = SparseSet<Component>();
    }

    template <typename... Components>
    EntityID registerEntity()
    {
        const DynamicBitSet entity(getComponentsBitSet<Components...>());

        _entityList.push_back(entity);
        return _entityList.size() - 1;
    }

    template <typename System>
    void registerSystem(std::unique_ptr<System> system)
    {
        static_assert(std::is_base_of<ASystem, System>::value, "System must inherit from ASystem");
        const size_t hashcode = typeid(System).hash_code();
        _systemView[hashcode] = std::move(system);
    }

    template <typename System>
    void applySystem()
    {
        static_assert(std::is_base_of<ASystem, System>::value, "System must inherit from ASystem");
        const size_t hashcode = typeid(System).hash_code();
        applySystem(hashcode);
    }

    void applyAllSystems();

    template <typename Component>
    ISparseSet &getComponent()
    {
        const size_t hashcode = typeid(Component).hash_code();
        return _componentView[hashcode];
    };

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

    template <typename Component>
    Component &getComponentOf(EntityID entity)
    {
        const ComponentID componentId = typeid(Component).hash_code();
        auto sparseSet = dynamic_cast<SparseSet<Component>>(_componentView.getDenseIndex(componentId));

        return sparseSet.get(entity);
    }

   private:
    std::vector<Entity> _entityList;
    SparseVectorView<ComponentID, ISparseSet> _componentView;
    SparseVectorView<SystemID, std::unique_ptr<ASystem>> _systemView;
};

}  // namespace rtecs
#include <bitset>

#include "ASystem.hpp"
#include "DynamicBitSet.hpp"
#include "rtecs/types.hpp"

namespace rtecs {

class ECS; // Forward declaration for system type

using Entity = BitMask;
using SystemID = size_t;
using ComponentID = decltype (typeid(ISparseSet).hash_code());

class ECS final {
private:

public:

    template <typename Component>
    void registerComponent() {
        const size_t hashcode = typeid(Component).hash_code();
        _componentMap[hashcode] = SparseSet<Component>();
    }

    EntityID registerEntity(Entity entity);
    SystemID registerSystem(ASystem system);

    void applySystem(SystemID systemId);
    void applyAllSystem();

    template <typename ...Components>
    std::vector<Components> getAllComponents();

    template <typename Component>
    Component &getComponentOf(EntityID entity) {
        const ComponentID componentId = typeid(Component).hash_code();
        auto sparseSet = dynamic_cast<SparseSet<Component>>(_componentMap[componentId]);

        return sparseSet.getComponent(entity);
    }

    std::vector<Entity> getAllEntities();

private:
    std::vector<Entity> _entityList;
    std::unordered_map<ComponentID, ISparseSet> _componentMap;
    std::vector<ISparseSet>
    std::vector<ASystem> _systemList;
};

}
