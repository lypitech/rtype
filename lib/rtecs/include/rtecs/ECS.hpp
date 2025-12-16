#pragma once

#include <memory>

#include "bitset/DynamicBitSet.hpp"
#include "logger/Logger.h"
#include "sparse/ISparseSet.hpp"
#include "sparse/SparseSet.hpp"
#include "sparse/SparseVectorView.hpp"
#include "sparse/View.hpp"
#include "systems/ASystem.hpp"

namespace rtecs {

using Entity = bitset::DynamicBitSet;
using EntityID = size_t;
using SystemID = size_t;

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
    void applySystem(SystemID id);

public:
    explicit ECS() = default;
    ECS(const ECS &) = delete;
    ECS &operator=(const ECS &) = delete;
    ECS(ECS &&) = default;
    ECS &operator=(ECS &&) = default;

    ~ECS() = default;

private:
    std::vector<Entity> _entityList;
};

}  // namespace rtecs
