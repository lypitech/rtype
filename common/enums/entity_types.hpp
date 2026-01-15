#pragma once

#include <string>
#include <unordered_map>

namespace entity {

enum class Type
{
    kPlayer = 0,
    kEnemy,
    kBullet,
};

static const std::unordered_map<std::string, Type> StringToType = {
    {"Player", Type::kPlayer}, {"Enemy", Type::kEnemy}, {"Bullet", Type::kBullet}};

static const std::unordered_map<Type, std::string> TypeToString = {
    {Type::kPlayer, "Player"}, {Type::kEnemy, "Enemy"}, {Type::kBullet, "Bullet"}};

}  // namespace entity
