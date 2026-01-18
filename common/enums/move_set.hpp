#pragma once

#include <string>
#include <unordered_map>

namespace move {

enum class Set
{
    kStraightSlow = 0,
    kZigZag,
    kHover,
    kWave
};

static const std::unordered_map<std::string, Set> StringToMoveSet = {
    {"straight_slow", Set::kStraightSlow},
    {"zigzag", Set::kZigZag},
    {"hover", Set::kHover},
    {"wave", Set::kWave}};

static const std::unordered_map<Set, std::string> MoveSetToString = {
    {Set::kStraightSlow, "straight_slow"},
    {Set::kZigZag, "zigzag"},
    {Set::kHover, "hover"},
    {Set::kWave, "wave"}};

}  // namespace move
