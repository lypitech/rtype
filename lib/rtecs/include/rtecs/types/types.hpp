#pragma once

#include <functional>

#include "rtecs/bitset/DynamicBitSet.hpp"

namespace rtecs::types {

using Entity = bitset::DynamicBitSet;
using EntityID = size_t;
using SystemID = size_t;

template <typename... T>
using System = std::function<void(T&... components)>;

/// The ComponentID is the hash code value (from typeid(ComponentType).hash_code())
using ComponentID = decltype(typeid(Entity).hash_code());

}  // namespace rtecs::types
