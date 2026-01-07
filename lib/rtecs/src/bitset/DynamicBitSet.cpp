#include "rtecs/bitset/DynamicBitSet.hpp"

#include <algorithm>
#include <ranges>
#include <sstream>
#include <vector>

#include "logger/Logger.h"

using namespace rtecs::bitset;

// =======================
//         BitRef
// =======================
DynamicBitSet::BitRef::BitRef(std::bitset<64>& b, const size_t bitIndex)
    : _block(b), _bitIndex(bitIndex)
{
}

DynamicBitSet::BitRef::operator bool() const { return _block[_bitIndex]; }

DynamicBitSet::BitRef& DynamicBitSet::BitRef::operator=(const bool value)
{
    _block[_bitIndex] = value;
    return *this;
}

bool DynamicBitSet::BitRef::operator==(const BitRef& other) const { return _block == other._block; }
bool DynamicBitSet::BitRef::operator==(const bool value) const { return _block[_bitIndex] == value; }

// =======================
//      DynamicBitSet
// =======================

DynamicBitSet::DynamicBitSet(const std::vector<uint8_t>& bytes)
{
    for (const unsigned char bit : bytes) {
        (*this)[bit] = true;
    }
}

DynamicBitSet::DynamicBitSet(const std::vector<std::bitset<64>>& bitsets)
    : _bitsets(bitsets)
{
}

void DynamicBitSet::applyOperation(const Operation& operation, DynamicBitSet& result, const DynamicBitSet& other) const
{
    const size_t currentCapacity = capacity();
    const size_t otherCapacity = other.capacity();
    const size_t limit = std::max(currentCapacity, otherCapacity);

    for (size_t i = 0; i < limit; i++) {
        const size_t block = DYN_BLOCK_INDEX(i);
        const size_t bitIndex = DYN_BIT_INDEX(i);
        const bool bitA = (i < currentCapacity) ? _bitsets[block][bitIndex] : false;
        const bool bitB = (i < otherCapacity) ? other._bitsets[block][bitIndex] : false;
        result[i] = operation(bitA, bitB);
    }
}

void DynamicBitSet::leftShift(size_t nb)
{
    for (; nb > 0; nb--) {
        for (auto it = _bitsets.begin(); it != _bitsets.end(); ++it) {
            if (it != _bitsets.begin() && (*it)[63] == true) {
                (*(it - 1))[0] = true;
            }
            *it <<= 1;
        }
    }
}

void DynamicBitSet::rightShift(size_t nb)
{
    for (; nb > 0; nb--) {
        for (auto it = _bitsets.rbegin(); it != _bitsets.rend(); ++it) {
            if (it != _bitsets.rbegin() && (*it)[0] == true) {
                (*(it - 1))[63] = true;
            }
            *it >>= 1;
        }
    }
}

std::pair<std::vector<uint64_t>, size_t> DynamicBitSet::serialize() const
{
    std::vector<uint64_t> indexes;
    size_t nbBits = 0;

    for (size_t i = 0; i < _bitsets.size() * 64; i++) {
        if ((*this)[i]) {
            indexes.push_back(i);
            nbBits = i + 1;
        }
    }
    return {indexes, nbBits};
}

std::string DynamicBitSet::toString(const std::string& sep) const
{
    std::stringstream stream;

    for (auto it = _bitsets.begin(); it != _bitsets.end(); ++it) {
        stream << it->to_string();
        if (it + 1 != _bitsets.end()) {
            stream << sep;
        }
    }
    return stream.str();
}

size_t DynamicBitSet::increase(const size_t size)
{
    for (size_t i = 0; i < size; i++) {
        _bitsets.emplace_back();
    }
    return capacity();
}

size_t DynamicBitSet::decrease(const size_t size)
{
    for (size_t i = 0; i < size; i++) {
        _bitsets.pop_back();
    }
    return capacity();
}

size_t DynamicBitSet::capacity() const { return _bitsets.size() * 64; }

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
    return std::ranges::all_of(_bitsets.begin(), _bitsets.end(),
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
    const Operation operation = [](const bool a, const bool b) { return a & b; };
    DynamicBitSet result(*this);

    applyOperation(operation, result, other);
    return result;
}

DynamicBitSet DynamicBitSet::operator|(const DynamicBitSet& other) const
{
    const Operation operation = [](const bool a, const bool b) { return a || b; };
    DynamicBitSet result(*this);

    applyOperation(operation, result, other);
    return result;
}

DynamicBitSet DynamicBitSet::operator^(const DynamicBitSet& other) const
{
    const Operation operation = [](const bool a, const bool b) { return a ^ b; };
    DynamicBitSet result(*this);

    applyOperation(operation, result, other);
    return result;
}

DynamicBitSet DynamicBitSet::operator~() const
{
    const Operation operation = [](const bool a, const bool) { return !a; };
    DynamicBitSet result(*this);

    applyOperation(operation, result, result);
    return result;
}

DynamicBitSet& DynamicBitSet::operator&=(const DynamicBitSet& other)
{
    const Operation operation = [](const bool a, const bool b) { return a && b; };

    applyOperation(operation, *this, other);
    return *this;
}

DynamicBitSet& DynamicBitSet::operator|=(const DynamicBitSet& other)
{
    const Operation operation = [](const bool a, const bool b) { return a || b; };

    applyOperation(operation, *this, other);
    return *this;
}

DynamicBitSet& DynamicBitSet::operator^=(const DynamicBitSet& other)
{
    const Operation operation = [](const bool a, const bool b) { return a ^ b; };

    applyOperation(operation, *this, other);
    return *this;
}

DynamicBitSet::BitRef DynamicBitSet::operator[](const size_t i)
{
    if (i >= capacity()) {
        _bitsets.resize(i / 64 + 1);
    }

    const size_t blockIndex = DYN_BLOCK_INDEX(i);
    const size_t bitIndex = DYN_BIT_INDEX(i);
    return BitRef{_bitsets[blockIndex], bitIndex};
}

bool DynamicBitSet::operator[](const size_t i) const
{
    if (i >= capacity()) {
        return false;
    }
    return _bitsets[DYN_BLOCK_INDEX(i)][DYN_BIT_INDEX(i)];
}

bool DynamicBitSet::operator==(const DynamicBitSet& other) const
{
    const size_t currentCapacity = capacity();
    const size_t otherCapacity = other.capacity();
    const size_t limit = std::max(currentCapacity, otherCapacity);

    for (size_t i = 0; i < limit; i++) {
        const bool bitA = (i < currentCapacity) ? _bitsets[i / 64][i % 64] : false;
        const bool bitB = (i < otherCapacity) ? other._bitsets[i / 64][i % 64] : false;
        if (bitA != bitB) {
            return false;
        }
    }
    return true;
}

DynamicBitSet DynamicBitSet::operator<<(const size_t nb) const
{
    DynamicBitSet bitset(*this);

    bitset.leftShift(nb);
    return bitset;
}

DynamicBitSet& DynamicBitSet::operator<<=(const size_t nb)
{
    leftShift(nb);
    return *this;
}

DynamicBitSet DynamicBitSet::operator>>(const size_t nb) const
{
    DynamicBitSet bitset(*this);

    bitset.rightShift(nb);
    return bitset;
}

DynamicBitSet& DynamicBitSet::operator>>=(const size_t nb)
{
    rightShift(nb);
    return *this;
}

std::ostream& operator<<(std::ostream& stream, const DynamicBitSet& ref)
{
    stream << "[" << ref.toString(" ") << "]";
    return stream;
}
