#include <algorithm>

#include "level_director.hpp"

namespace level {

const wave::Archetype& Director::pickRandomWave()
{
    std::vector<const wave::Archetype*> affordable;
    size_t sum = 0;

    for (const auto& wave : _wavePool) {
        if (_credits >= wave.difficultyCost) {
            affordable.push_back(&wave);
            sum += wave.weight;
        }
    }
    if (sum == 0) {
        return _wavePool[0];
    }
    std::uniform_int_distribution<size_t> dist(1, sum);
    size_t randomValue = dist(_rng);

    for (const auto* wave : affordable) {
        if (randomValue <= wave->weight) {
            return *wave;
        }
        randomValue -= wave->weight;
    }
    return *affordable.back();
}

}  // namespace level
