#pragma once
#include <nlohmann/json_fwd.hpp>
#include <random>
#include <vector>

#include "archetype.hpp"

class Lobby;

namespace level {

static constexpr float BASE_INCOME = 2.0f;
static constexpr float INCOME_MULTIPLIER = 2.0f;

class Director
{
public:
    Director() = default;
    void load(const std::string& config);
    void update(float dt,
                Lobby& lobby);

private:
    std::vector<wave::Archetype> _wavePool;
    std::mt19937 _rng;
    int _credits;
    float _currentDifficulty;
    float _gameTime;
    std::vector<wave::Active> _activeWaves;

    void pickRandomWaves();
    std::vector<const wave::Archetype*> getPickedWaves();
    void parseArchetypes(const nlohmann::json& data);
    void pickNewWaveIfNeeded();
};

}  // namespace level
