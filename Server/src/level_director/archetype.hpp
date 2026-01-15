#pragma once
#include <string>
#include <vector>

#include "enums/entity_types.hpp"

namespace level {

/**
 * @brief A group of enemies
 */
struct Enemy
{
    entity::Type type;        ///< The type of enemy (must be in the entity::Type enum).
    std::string patternName;  ///< The name of the movement pattern.
    int count;                ///< The number of entities in this group.
};

namespace wave {

/**
 * @brief The generic type of wave.
 */
struct Archetype
{
    std::string name;    ///< The name of this archetype.
    int difficultyCost;  ///< The cost for picking this wave archetype.
    size_t weight;       ///< The chances of this to be picked.

    float spawnInterval;  ///< The interval between the spawn of each of the enemies.
    float postWaveDelay;  ///< The interval to wait after spawning this wave.

    std::vector<Enemy> enemies;  ///< A vector of @code Enemy@endcode to composing the wave.
};

/**
 * @brief A container for the active wave.
 */
struct Active
{
    const Archetype* archetype;    ///< A pointer to the current active wave archetype.
    float timer = 0.0f;            ///< The timer since last spawn.
    size_t currentGroupIndex = 0;  ///< The current index of the group of enemy to spawn.
    int spawnedInGroup = 0;        ///< The number of entities spawned for the current group.
    bool isFinished = false;       ///< Whether the wave has spawned all of its enemies.
};

}  // namespace wave
}  // namespace level
