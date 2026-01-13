#include "level_director.hpp"

#include <nlohmann/json.hpp>

#include "lobby/lobby.hpp"
std::vector<level::Enemy> parseEnemies(const nlohmann::json& data)
{
    if (!data.contains("enemies") || !data["enemies"].is_array()) {
        LOG_CRIT("[ERROR]: you must specify an array of enemies.");
    }
    std::vector<level::Enemy> enemies;
    for (const auto& e : data["enemies"]) {
        level::Enemy enemy;
        enemy.type = e.at("type");
        enemy.patternName = e.at("pattern");
        enemy.count = e.at("count");
        enemies.push_back(enemy);
    }
    return enemies;
}

namespace level {
void Director::parseArchetypes(const nlohmann::json& data)
{
    if (!data.contains("wave") || !data["wave"].is_array()) {
        LOG_CRIT("[ERROR]: Wave object should be an array.");
        return;
    }
    for (const auto& wave : data["wave"]) {
        wave::Archetype archetype;
        archetype.name = wave.at("id");
        archetype.difficultyCost = wave.at("cost");
        archetype.spawnTime = wave.at("time");
        archetype.spawnInterval = wave.at("interval");
        archetype.enemies = parseEnemies(wave);
        _wavePool.push_back(archetype);
    }
    using arch = const wave::Archetype;
    std::sort(_wavePool.begin(), _wavePool.end(), [](arch& a, arch& b) {
        return a.spawnTime < b.spawnTime;
    });
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
}  // namespace level
