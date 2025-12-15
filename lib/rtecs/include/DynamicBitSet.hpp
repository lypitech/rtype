#pragma once

#include <bitset>
#include <cstdint>
#include <vector>

namespace rtecs {

/**
 * @file DynamicBitSet.hpp
 * @brief A small, dynamically-resizable bitset used by the ECS.
 */

class DynamicBitSet
{
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

    /** @brief Default constructor creating an empty bitset. */
    explicit DynamicBitSet() = default;

    /**
     * @brief Construct a `DynamicBitSet` from a byte array.
     *
     * @param bytes The byte array containing the index of activated bytes.
     */
    explicit DynamicBitSet(const std::vector<uint8_t> &bytes);

    /** @brief Serialize the bitset to a byte array. */
    [[nodiscard]]
    std::pair<std::vector<uint8_t>, size_t> toBytes() const;

    /** @return The number of bits tracked by the bitset. */
    [[nodiscard]] size_t size() const { return _nbits; }

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

private:
    std::vector<std::bitset<64>> _bitsets;
    size_t _nbits = 0;  ///< The index of the last activated bit.
};

}  // namespace rtecs
