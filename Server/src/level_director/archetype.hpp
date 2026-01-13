#pragma once
#include <string>
#include <vector>

#include "enums/entity_types.hpp"

namespace level {

struct Enemy
{
    entity::Type type;        // "scout", "tank"
    std::string patternName;  // "zigzag", "homing"
    int count;                // How many of this specific enemy in this group?
};

namespace wave {

struct Archetype
{
    std::string name;
    int difficultyCost;
    size_t weight;

    float spawnInterval;
    float postWaveDelay;

    std::vector<Enemy> enemies;
};

struct Active
{
    const Archetype* archetype;
    float timer = 0.0f;
    size_t currentGroupIndex = 0;
    int spawnedInGroup = 0;
    bool isFinished = false;
};

}  // namespace wave
}  // namespace level
