#include "level_director.hpp"

#include <nlohmann/json.hpp>

#include "enums/entity_types.hpp"
#include "lobby/lobby.hpp"

std::vector<level::Enemy> parseEnemies(const nlohmann::json& data)
{
    if (!data.contains("enemies") || !data["enemies"].is_array()) {
        LOG_CRIT("[ERROR]: you must specify an array of enemies.");
    }
    std::vector<level::Enemy> enemies;
    for (const auto& e : data["enemies"]) {
        level::Enemy enemy;
        enemy.type = entity::StringToType.at(e.at("type"));
        enemy.patternName = e.at("pattern");
        enemy.count = e.at("count");
        enemies.push_back(enemy);
    }
    return enemies;
}

namespace level {
void Director::parseArchetypes(const nlohmann::json& data)
{
    if (!data.contains("waves") || !data["waves"].is_array()) {
        LOG_CRIT("[ERROR]: Wave object should be an array.");
        return;
    }
    for (const auto& wave : data["waves"]) {
        wave::Archetype archetype;
        archetype.name = wave.at("name");
        archetype.difficultyCost = wave.at("difficultyCost");
        archetype.weight = wave.at("weight");
        archetype.spawnInterval = wave.at("spawnInterval");
        archetype.postWaveDelay = wave.at("postWaveDelay");
        archetype.enemies = parseEnemies(wave);
        _wavePool.push_back(archetype);
    }
}

void Director::load(const std::string& config)
{
    std::ifstream f(config);
    if (!f.is_open()) {
        LOG_CRIT("[Error] Could not open wave config file: {}", config);
        return;
    }
    using json = nlohmann::json;
    try {
        const json data = nlohmann::json::parse(f);
        parseArchetypes(data);
    } catch (nlohmann::json::parse_error& e) {
        LOG_CRIT("{} at {}", e.what(), e.byte);
    } catch (const json::type_error& e) {
        LOG_CRIT("[Error] JSON Type mismatch: {}", e.what());
    }
}

void Director::update(const float dt,
                      Lobby& lobby)
{
    _gameTime += dt;

    const float incomeRate = BASE_INCOME + (_gameTime / 60.0f) * INCOME_MULTIPLIER;
    _credits += incomeRate * dt;

    pickNewWaveIfNeeded();
    if (_activeWaves.empty()) {
        return;
    }
    for (int i = _activeWaves.size() - 1; i >= 0; --i) {
        wave::Active& wave = _activeWaves[i];

        if (wave.isFinished) {
            _activeWaves.erase(_activeWaves.begin() + i);
            continue;
        }

        wave.timer += dt;

        if (wave.timer < wave.archetype->spawnInterval) {
            continue;
        }

        wave.timer -= wave.archetype->spawnInterval;

        const auto& group = wave.archetype->enemies[wave.currentGroupIndex];

        // TODO: Make the entities spawn off screen.
        std::uniform_real_distribution yDist(200.0f, 950.0f);
        std::uniform_real_distribution xDist(150.0f, 1200.0f);
        lobby.spawnEntity<components::Position, components::Type>(
            {xDist(_rng), yDist(_rng)}, {group.type});

        wave.spawnedInGroup++;

        if (wave.spawnedInGroup >= group.count) {
            wave.spawnedInGroup = 0;
            wave.currentGroupIndex++;

            if (wave.currentGroupIndex >= wave.archetype->enemies.size()) {
                wave.isFinished = true;
            }
        }
    }
}

}  // namespace level
