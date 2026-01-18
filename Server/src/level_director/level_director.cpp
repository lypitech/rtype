#include "level_director.hpp"

#include <algorithm>
#include <nlohmann/json.hpp>

#include "components/hitbox.hpp"
#include "components/type.hpp"
#include "enums/entity_types.hpp"
#include "enums/player_state.hpp"
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

void Director::load(const std::string& waveConfig)
{
    std::ifstream f(waveConfig);
    if (!f.is_open()) {
        LOG_CRIT("[Error] Could not open wave config file: {}", waveConfig);
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
    LOG_TRACE_R2("Gained {} credits.", incomeRate * dt);

    pickNewWaveIfNeeded();
    if (_activeWaves.empty()) {
        LOG_TRACE_R2("No wave picked, Skipping frame...");
        return;
    }
    std::vector<components::Position> playerPos = lobby.getPlayerPositions();
    std::ranges::sort(playerPos, [](const components::Position& a, const components::Position& b) {
        return a.x < b.x;
    });
    const float minOffsetX = playerPos.back().x + 400;
    for (int i = _activeWaves.size() - 1; i >= 0; --i) {
        wave::Active& wave = _activeWaves[i];

        if (wave.isFinished) {
            LOG_TRACE_R2("Waved ended. Removing wave from activeWaves...");
            _activeWaves.erase(_activeWaves.begin() + i);
            continue;
        }

        wave.timer += dt;

        if (wave.timer < wave.archetype->spawnInterval) {
            continue;
        }

        LOG_TRACE_R2(
            "Spawning a wave of {}, timer is {} seconds.", wave.archetype->name, wave.timer);
        wave.timer -= wave.archetype->spawnInterval;

        const auto& group = wave.archetype->enemies[wave.currentGroupIndex];

        std::uniform_real_distribution yDist(200.0f, 950.0f);
        std::uniform_real_distribution xDist(150.0f, 1200.0f);
        float x = minOffsetX + xDist(_rng);
        float y = yDist(_rng);
        LOG_TRACE_R2("Spawning {} {}/{} from group {}/{} at ({}, {})",
                     entity::TypeToString.at(group.type),
                     wave.spawnedInGroup + 1,
                     group.count,
                     wave.currentGroupIndex + 1,
                     wave.archetype->enemies.size(),
                     x,
                     y);
        using namespace components;
        lobby.spawnEntity<Position, Type, Hitbox, Velocity, Collision, Value, State, MoveSet>(
            {x, y},
            {group.type},
            typeToHitbox(group.type),
            typeToVelocity(group.type),
            {},
            typeToValue(group.type),
            {entity::state::EntityAlive},
            {nameToMoveSet(group.patternName)});

        wave.spawnedInGroup++;

        if (wave.spawnedInGroup >= group.count) {
            LOG_TRACE_R2(
                "Spawned all enemy of group #{}, going to next group...", wave.currentGroupIndex);
            wave.spawnedInGroup = 0;
            wave.currentGroupIndex++;

            if (wave.currentGroupIndex >= wave.archetype->enemies.size()) {
                wave.isFinished = true;
            }
        }
    }
}

void Director::restart()
{
    _activeWaves.clear();
    _credits = 0;
    _currentDifficulty = 0;
    _gameTime = 0;
}

}  // namespace level
