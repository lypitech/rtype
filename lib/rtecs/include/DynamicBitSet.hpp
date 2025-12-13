#pragma once

#include <bitset>
#include <vector>

namespace rtecs {

/**
 * @file DynamicBitSet.hpp
 * @brief A small, dynamically-resizable bitset used by the ECS.
 */

class DynamicBitSet
{
    std::vector<std::bitset<64>> _bitsets;
    size_t _nbits = 0;

public:
    /**
     * @brief Proxy type used for mutable access to a single bit.
     *
     * Allows assignment like `bits[index] = true;` while hiding the underlying
     * block and mask logic.
     */
    class BitRef
    {
        std::bitset<64> &block;
        std::bitset<64> mask;

       public:
        explicit BitRef(std::bitset<64> &b, std::bitset<64> m);

        /** @return bit value as boolean */
        explicit operator bool() const;

        /** @brief Set or clear the referenced bit. */
        BitRef &operator=(bool v);
        bool operator==(const BitRef &) const;
    };

    /** @return true if any bit is set. */
    [[nodiscard]] bool any() const;
    /** @return true if all tracked bits are set. */
    [[nodiscard]] bool all() const;
    /** @return true if no bits are set. */
    [[nodiscard]] bool none() const;

    /** @brief Clear all bits and release allocated blocks. */
    void clear();

    /** @brief Bitwise AND—returns a new bitset. */
    DynamicBitSet operator&(const DynamicBitSet &other) const;
    /** @brief Bitwise OR—returns a new bitset. */
    DynamicBitSet operator|(const DynamicBitSet &other) const;
    /** @brief Mutable access to bit at index `i`. */
    BitRef operator[](size_t i);
    /** @brief Read-only access to bit at index `i`. */
    bool operator[](size_t i) const;
    /** @brief Compare two bitsets for equality. */
    bool operator==(const DynamicBitSet &) const;
};

}  // namespace rtecs
