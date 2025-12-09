#pragma once

#include <functional>
#include <optional>

#include "DynamicBitSet.hpp"

namespace rtecs {

// uint64_t is an equivalent to size_t, but it is more stable.
// It does not rely on the platform contrary to size_t.
using EntityID = size_t;

template <typename T>
using OptionalRef = std::optional<std::reference_wrapper<T>>;
template <typename T>
using OptionalCRef = std::optional<std::reference_wrapper<const T>>;

static constexpr EntityID kNullEntity = 0;

}  // namespace rtecs
