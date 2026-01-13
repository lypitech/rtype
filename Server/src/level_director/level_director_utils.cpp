#include <algorithm>

#include "level_director.hpp"

namespace level {

std::vector<wave::Archetype*> Director::pickRandomWaves()
{
    std::vector<wave::Archetype*> candidates;
    size_t sum = 0;
    const size_t minSpend = _credits / 2;

    for (const auto& wave : _wavePool) {
        if (wave.difficultyCost <= _credits && wave.difficultyCost >= minSpend) {
            candidates.push_back(const_cast<std::vector<wave::Archetype*>::value_type>(&wave));
            sum += wave.weight;
        }
    }

    if (candidates.empty()) {
        sum = 0;
        for (const auto& wave : _wavePool) {
            if (wave.difficultyCost <= _credits) {
                candidates.push_back(const_cast<std::vector<wave::Archetype*>::value_type>(&wave));
                sum += wave.weight;
            }
        }
    }

    if (candidates.empty() || sum == 0) {
        return {};
    }

    std::uniform_int_distribution<size_t> dist(1, sum);
    std::vector<const wave::Archetype*> bought;
    size_t randomValue = dist(_rng);

    for (const auto* wave : candidates) {
        if (randomValue <= wave->weight) {
            bought.push_back(wave);
        }
        randomValue -= wave->weight;
    }

    return {candidates.back()};
}

}  // namespace level
