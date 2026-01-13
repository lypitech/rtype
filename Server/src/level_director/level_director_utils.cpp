#include <algorithm>

#include "level_director.hpp"

namespace level {

void Director::pickRandomWaves()
{
    _currentWaves.clear();
    while (_credits > 0) {
        std::vector<const wave::Archetype*> candidates;
        size_t totalWeight = 0;
        const double minSpend = _credits * 0.5;

        for (const auto& wave : _wavePool) {
            if (wave.difficultyCost <= _credits && wave.difficultyCost >= minSpend) {
                candidates.push_back(&wave);
                totalWeight += wave.weight;
            }
        }

        if (candidates.empty()) {
            totalWeight = 0;
            for (const auto& wave : _wavePool) {
                if (wave.difficultyCost <= _credits) {
                    candidates.push_back(&wave);
                    totalWeight += wave.weight;
                }
            }
        }

        if (candidates.empty() || totalWeight == 0) {
            break;
        }

        std::uniform_int_distribution<size_t> dist(1, totalWeight);
        size_t randomValue = dist(_rng);
        const wave::Archetype* selected = nullptr;

        for (const auto* wave : candidates) {
            if (randomValue <= wave->weight) {
                selected = wave;
                break;
            }
            randomValue -= wave->weight;
        }

        if (!selected) {
            selected = candidates.back();
        }

        _currentWaves.push_back(selected);
        _credits -= selected->difficultyCost;
    }
}

}  // namespace level
