#pragma once
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
