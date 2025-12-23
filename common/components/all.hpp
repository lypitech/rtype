#pragma once

#include "Transform.hpp"
#include "position.hpp"
#include "rect.hpp"

namespace components {

using GameComponents = rteng::ComponentsList<Position, Transform, Rectangle>;

}
