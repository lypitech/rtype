#include <algorithm>

#include "level_director.hpp"
#include "logger/Logger.h"

namespace level {

void Director::pickRandomWaves()
{
    std::vector<const wave::Archetype*> candidates = getPickedWaves();

    for (const auto& arch : candidates) {
        wave::Active instance;
        instance.archetype = arch;
        std::uniform_real_distribution<float> delay(0.0f, 2.0f);
        instance.timer = -delay(_rng);
        _activeWaves.push_back(instance);
    }
}

std::vector<const wave::Archetype*> Director::getPickedWaves()
{
    std::vector<const wave::Archetype*> waves;

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

        waves.push_back(selected);
        _credits -= selected->difficultyCost;
    }
    return waves;
}

void Director::pickNewWaveIfNeeded()
{
    if (!_activeWaves.empty()) {
        return;
    }
    pickRandomWaves();
}

}  // namespace level
