#pragma once

#include <functional>
#include <optional>

#include "DynamicBitSet.hpp"

namespace rtecs {

template <typename T>
using OptionalRef = std::optional<std::reference_wrapper<T>>;
template <typename T>
using OptionalCRef = std::optional<std::reference_wrapper<const T>>;

}  // namespace rtecs
