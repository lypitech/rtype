#pragma once

#include <memory>

#include "ASystem.hpp"
#include "DynamicBitSet.hpp"
#include "ISparseSet.hpp"
#include "SparseSet.hpp"
#include "SparseVectorView.hpp"

namespace rtecs {

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
    void emplaceComponent(EntityID id, Component &&component)
    {
        const size_t hashcode = typeid(std::remove_reference_t<Component>).hash_code();

        auto &sparseSet = static_cast<SparseSet<std::remove_reference_t<Component>> &>(*_componentView[hashcode]);

        sparseSet.put(id, std::forward<Component>(component));
    }

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

    template <typename... Components>
    static std::unique_ptr<ECS> createWithComponents()
    {
        std::unique_ptr<ECS> e = std::make_unique<ECS>();
        (e->registerComponent<Components>(), ...);
        return e;
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
        _componentView.emplace(hashcode, std::make_unique<SparseSet<Component>>());
    }

    template <typename... Components>
    EntityID registerEntity(Components &&...components)
    {
        const DynamicBitSet entity = (getComponentBitSet<std::remove_reference_t<Components>>() | ...);

        _entityList.push_back(entity);

        const EntityID id = _entityList.size() - 1;

        (emplaceComponent(id, std::forward<Components>(components)), ...);
        return id;
    }

    template <typename... Components>
    EntityID registerEntity()
    {
        const DynamicBitSet entity(getComponentsBitSet<Components...>());

        _entityList.push_back(entity);
        return _entityList.size() - 1;
    }

    template <typename Component>
    bool hasEntityComponent(EntityID entityId)
    {
        const DynamicBitSet &entity = _entityList[entityId];
        const DynamicBitSet componentBitset = getComponentBitSet<Component>();

        return (entity & componentBitset) == componentBitset;
    }

    template <typename System>
    void registerSystem(std::unique_ptr<System> system)
    {
        static_assert(std::is_base_of_v<ASystem, System>, "System must inherit from ASystem");
        const size_t hashcode = typeid(System).hash_code();
        _systemView.emplace(hashcode, std::move(system));
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
        return *_componentView[hashcode];
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
    SparseVectorView<ComponentID, std::unique_ptr<ISparseSet>> _componentView;
    SparseVectorView<SystemID, std::unique_ptr<ASystem>> _systemView;
};

}  // namespace rtecs
