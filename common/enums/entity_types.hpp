#pragma once
#include <unordered_map>

namespace entity {

enum class Type
{
    kPlayer = 1,
    kEnemy,
};

static const std::unordered_map<std::string, Type> StringToType = {
    {"Player", Type::kPlayer}, {"Enemy", Type::kEnemy}, {"Bullet", Type::kBullet}};

}  // namespace entity
