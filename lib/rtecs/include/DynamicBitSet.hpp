#pragma once

#include <bitset>
#include <vector>

namespace rtecs {

class DynamicBitSet {
    std::vector<std::bitset<64>> _bitsets;
    size_t _nbits = 0;

   public:
    class BitRef {
        std::bitset<64> &block;
        std::bitset<64> mask;

       public:
        explicit BitRef(std::bitset<64> &b, std::bitset<64> m);

        explicit operator bool() const;

        BitRef &operator=(bool v);
        bool operator==(const BitRef &) const;
    };

    [[nodiscard]] bool any() const;
    [[nodiscard]] bool all() const;
    [[nodiscard]] bool none() const;

    void clear();

    DynamicBitSet operator&(const DynamicBitSet &other) const;
    DynamicBitSet operator|(const DynamicBitSet &other) const;
    BitRef operator[](size_t i);
    bool operator[](size_t i) const;
    bool operator==(const DynamicBitSet &) const;
};

}  // namespace rtecs
