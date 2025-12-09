#include "DynamicBitSet.hpp"

#include <algorithm>

using namespace rtecs;

DynamicBitSet::BitRef::BitRef(std::bitset<64>& b, const std::bitset<64> m)
    : block(b), mask(m) {}

DynamicBitSet::BitRef::operator bool() const { return (block & mask).any(); }

DynamicBitSet::BitRef& DynamicBitSet::BitRef::operator=(const bool v) {
    if (v)
        block |= mask;
    else
        block &= ~mask;
    return *this;
}

bool DynamicBitSet::BitRef::operator&(const BitRef& other) {}

bool DynamicBitSet::any() const {
    return std::ranges::any_of(
        _bitsets.begin(), _bitsets.end(),
        [](const std::bitset<64> bitset) { return bitset.any(); });
};

bool DynamicBitSet::all() const {
    return std::ranges::all_of(
        _bitsets.begin(), _bitsets.end(),
        [](const std::bitset<64> bitset) { return bitset.all(); });
}

bool DynamicBitSet::none() const {
    return std::ranges::none_of(
        _bitsets.begin(), _bitsets.end(),
        [](const std::bitset<64> bitset) { return bitset.none(); });
};

void DynamicBitSet::clear() {
    for (auto& bitset : _bitsets) {
        bitset.reset();
    }
}

DynamicBitSet DynamicBitSet::operator&(DynamicBitSet& other) const {
    DynamicBitSet result;

    for (size_t i = 0; i < std::min(_nbits, other._nbits); i++) {
        result[i] = this[i] & other[i];
    }
    return result;
}

DynamicBitSet DynamicBitSet::operator|(DynamicBitSet& other) const {}

DynamicBitSet::BitRef DynamicBitSet::operator[](const size_t i) {
    return BitRef{_bitsets[i / 64], std::bitset<64>(i % 64)};
}

bool DynamicBitSet::operator[](const size_t i) const {
    return _bitsets[i / 64][i % 64];
}
