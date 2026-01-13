#pragma once
#include <nlohmann/json_fwd.hpp>
#include <random>
#include <vector>

#include "archetype.hpp"

class Lobby;

namespace level {

class Director
{
public:
    void load(const std::string& config);
    void update(float dt,
                Lobby& lobby);

private:
    std::vector<wave::Archetype> _wavePool;
    std::mt19937 _rng;

    void parseArchetypes(const nlohmann::json& data);
};

}  // namespace level
