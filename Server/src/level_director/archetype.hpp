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
    std::string name;       // "Swarm_A" (Good for debugging logs)
    size_t difficultyCost;  // The price to "buy" this wave
    size_t weight;          // Probability chance (Higher = more frequent)

    float spawnInterval;  // Time to wait between individual enemy spawns
    float postWaveDelay;  // Time to wait AFTER this wave finishes before the next starts

    std::vector<Enemy> enemies;
};

}  // namespace wave
}  // namespace level
