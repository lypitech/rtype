#pragma once

#include <functional>
#include <limits>
#include <optional>

#include "rtecs/bitset/DynamicBitSet.hpp"

namespace rtecs::types {

using SystemID = size_t;

using Entity = bitset::DynamicBitSet;
using EntityID = size_t;
constexpr EntityID NullEntityID = std::numeric_limits<EntityID>::max();

template <typename... T>
using System = std::function<void(T&... components)>;

/// The ComponentID is the hash code value (from typeid(ComponentType).hash_code())
using ComponentID = decltype(typeid(Entity).hash_code());

template <typename T>
using OptionalRef = std::optional<std::reference_wrapper<T>>;
template <typename T>
using OptionalCRef = std::optional<std::reference_wrapper<const T>>;

}  // namespace rtecs::types
