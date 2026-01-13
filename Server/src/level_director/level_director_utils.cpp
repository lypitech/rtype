#include <algorithm>

#include "level_director.hpp"

namespace level {

const wave::Archetype* Director::pickRandomWave()
{
    std::vector<const wave::Archetype*> candidates;
    size_t sum = 0;
    const size_t minSpend = _credits / 2;

    for (const auto& wave : _wavePool) {
        if (wave.difficultyCost <= _credits && wave.difficultyCost >= minSpend) {
            candidates.push_back(&wave);
            sum += wave.weight;
        }
    }

    if (candidates.empty()) {
        sum = 0;
        for (const auto& wave : _wavePool) {
            if (wave.difficultyCost <= _credits) {
                candidates.push_back(&wave);
                sum += wave.weight;
            }
        }
    }

    if (candidates.empty() || sum == 0) {
        return nullptr;
    }

    std::uniform_int_distribution<size_t> dist(1, sum);
    size_t randomValue = dist(_rng);

    for (const auto* wave : candidates) {
        if (randomValue <= wave->weight) {
            return wave;
        }
        randomValue -= wave->weight;
    }

    return candidates.back();
}

}  // namespace level
