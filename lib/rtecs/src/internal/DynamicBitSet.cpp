#include "DynamicBitSet.hpp"

#include <algorithm>
#include <ranges>

using namespace rtecs;

// =======================
//         BitRef
// =======================
DynamicBitSet::BitRef::BitRef(std::bitset<64>& b, const std::bitset<64> m)
    : block(b), mask(m)
{
}

DynamicBitSet::BitRef::operator bool() const { return (block & mask).any(); }

DynamicBitSet::BitRef& DynamicBitSet::BitRef::operator=(const bool v)
{
    if (v) {
        block |= mask;
    } else {
        block &= ~mask;
    }
    return *this;
}

bool DynamicBitSet::BitRef::operator==(const BitRef& other) const { return block == other.block; }

// =======================
//      DynamicBitSet
// =======================
bool DynamicBitSet::any() const
{
    return std::ranges::any_of(_bitsets.begin(), _bitsets.end(),
                               [](const std::bitset<64> bitset) { return bitset.any(); });
};

bool DynamicBitSet::all() const
{
    return std::ranges::all_of(_bitsets.begin(), _bitsets.end(),
                               [](const std::bitset<64> bitset) { return bitset.all(); });
}

bool DynamicBitSet::none() const
{
    return std::ranges::none_of(_bitsets.begin(), _bitsets.end(),
                                [](const std::bitset<64> bitset) { return bitset.none(); });
};

void DynamicBitSet::clear()
{
    for (auto& bitset : _bitsets) {
        bitset.reset();
    }
}

DynamicBitSet DynamicBitSet::operator&(const DynamicBitSet& other) const
{
    DynamicBitSet result;

    for (size_t i = 0; i < std::min(_nbits, other._nbits); i++) {
        result[i] = _bitsets[i / 64][i % 64] && other._bitsets[i / 64][i % 64];
    }
    return result;
}

DynamicBitSet DynamicBitSet::operator|(const DynamicBitSet& other) const
{
    DynamicBitSet result;

    for (size_t i = 0; i < std::min(_nbits, other._nbits); i++) {
        result[i] = _bitsets[i / 64][i % 64] || other._bitsets[i / 64][i % 64];
    }
    return result;
}

DynamicBitSet::BitRef DynamicBitSet::operator[](const size_t i)
{
    const size_t blockIndex = i / 64;

    if (blockIndex >= _bitsets.size()) {
        _bitsets.resize(blockIndex + 1);
    }
    if (i >= _nbits) {
        _nbits = i + 1;
    }
    std::bitset<64> mask;
    mask.set(i % 64);
    return BitRef{_bitsets[blockIndex], mask};
}

bool DynamicBitSet::operator[](const size_t i) const
{
    if (i >= _nbits) {
        return false;
    }
    return _bitsets[i / 64][i % 64];
}

bool DynamicBitSet::operator==(const DynamicBitSet& other) const
{
    if (other._nbits != _nbits) {
        return false;
    }

    for (const auto& [a, b] : std::views::zip(_bitsets, other._bitsets)) {
        if (a != b) {
            return false;
        }
    }
    return true;
}
