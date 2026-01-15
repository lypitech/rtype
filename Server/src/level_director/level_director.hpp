#pragma once
#include <nlohmann/json_fwd.hpp>
#include <random>
#include <vector>

#include "archetype.hpp"

class Lobby;

namespace level {

static constexpr float BASE_INCOME = 2.0f;
static constexpr float INCOME_MULTIPLIER = 2.0f;

/**
 * @class level::Director
 * @brief Loads a JSON config and generates waves of enemy depending on the configuration file.
 */
class Director
{
public:
    Director() = default;
    /**
     * @brief Loads a set of waves to be generated based on a configuration file.
     * @param waveConfig The path to a wave configuration file.
     */
    void load(const std::string& waveConfig);

    /**
     * @brief Spawns new entities when needed.
     * @param dt The @code supposed@endcode elapsed time since last call.
     * @param lobby A reference to the calling lobby in which create the entities.
     */
    void update(float dt,
                Lobby& lobby);

private:
    std::vector<wave::Archetype> _wavePool;
    std::mt19937 _rng{1234};
    float _credits;
    float _currentDifficulty;
    float _gameTime;
    std::vector<wave::Active> _activeWaves;

    /**
     * @brief This does what you think it does.
     */
    void pickRandomWaves();
    /**
     * @brief This does what you think it does.
     */
    std::vector<const wave::Archetype*> getPickedWaves();
    /**
     * @brief Parses A @code nlohmann::json@endcode data object and fills up the wavePool.
     * @param data A reference to a @code nlohmann::json@endcode data object.
     */
    void parseArchetypes(const nlohmann::json& data);
    /**
     * @brief This does what you think it does.
     */
    void pickNewWaveIfNeeded();
};

}  // namespace level
