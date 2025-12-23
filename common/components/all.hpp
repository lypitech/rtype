#pragma once
#include "position.hpp"
#include "rect.hpp"
#include "rteng.hpp"

namespace components {

using GameComponents = rteng::ComponentsList<Position, Rectangle
                                             // Add new components here...
                                             >;

}
