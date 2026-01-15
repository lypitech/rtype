#pragma once

#include <bitset>
#include <cstdint>
#include <functional>
#include <vector>

namespace rtecs::bitset {

/**
 * @file DynamicBitSet.hpp
 * @brief A small, dynamically-resizable bitset used by the ECS.
 */

#define BITSET_CAPACITY 64
#define DYN_BLOCK_INDEX(i) (i / BITSET_CAPACITY)
#define BIT_INDEX(i) (i % BITSET_CAPACITY)
#define DYN_BIT_INDEX(i) (BITSET_CAPACITY - 1 - (i % BITSET_CAPACITY))

class DynamicBitSet
{
private:
    std::vector<std::bitset<64>> _bitsets;
    size_t _nbits = 0;  ///< The index of the last activated bit.

    using Operation = std::function<uint64_t(bool a, bool b)>;
    void applyOperation(const Operation &operation,
                        DynamicBitSet &result,
                        const DynamicBitSet &other) const;
    void leftShift(size_t nb);
    void rightShift(size_t nb);

public:
    /**
     * @brief Proxy type used for mutable access to a single bit.
     *
     * Allows assignment like `bits[index] = true;` while hiding the underlying
     * block and mask logic.
     */
    class BitRef
    {
        std::bitset<64> &_block;
        size_t _bitIndex;

    public:
        explicit BitRef(std::bitset<64> &b,
                        size_t bitIndex);

        /** @return The bit value as boolean */
        explicit operator bool() const;

        /**
         * @brief Set the referenced bit.
         *
         * @param value The new bit value (true or false).
         * @return A reference to the DynamicBitSet::BitRef.
         */
        BitRef &operator=(bool value);

        /**
         * @brief Compare two bits from a DynamicBitSet::BitRef.
         *
         * @param other The other bit, encapsulated in a DynamicBitSet::BitRef.
         * @return `true` if the bits are equals, `false` otherwise.
         */
        bool operator==(const BitRef &other) const;

        /**
         * @brief Check if the DynamicBitSet::BitRef is `true` or `false`.
         *
         * @param value `true` to check if the DynamicBitSet::BitRef is enable, `false` otherwise.
         * @return `true` if the bit is enable, `false` otherwise.
         */
        bool operator==(bool value) const;
    };

    /** @brief Default constructor creating an empty bitset. */
    explicit DynamicBitSet() = default;

    /**
     * @brief Construct a copy of a bitset.
     *
     * @param ref The bitset reference to copy.
     */
    DynamicBitSet(const DynamicBitSet &ref) = default;

    /**
     * @brief Construct a `DynamicBitSet` from an array of byte.
     *
     * @param bytes The byte array containing the index of activated bytes.
     */
    explicit DynamicBitSet(const std::vector<uint8_t> &bytes);

    /**
     * @brief Construct a `DynamicBitSet` from an array of std::bitset<64>.
     *
     * @param bitsets The std::bitset<64> array.
     */
    explicit DynamicBitSet(const std::vector<std::bitset<64>> &bitsets);

    /**
     * @brief Construct a `DynamicBitSet` from a bit array of uint64_t.
     *
     * @param blocks The bit array of uint64_t.
     */
    explicit DynamicBitSet(const std::vector<uint64_t> &blocks);

    /**
     * @brief Serialize the bitset to a byte array.
     * @return The indexes of the activated bits.
     */
    [[nodiscard]]
    std::vector<uint64_t> serialize() const;

    /**
     * @brief Deserialize a bitset.
     *
     * @param indexes The indexes of the activated bits.
     * @return An instance of a DynamicBitSet.
     */
    static DynamicBitSet deserialize(const std::vector<uint64_t> &indexes);

    /**
     * @brief Get the string representation of the DynamicBitSet.
     * @return A string representation of the DynamicBitSet.
     */
    [[nodiscard]]
    std::string toString(const std::string &sep = "\n") const;

    /**
     * @brief Increase the capacity of the DynamicBitSet by `64 * size` bits.
     *
     * @note The new bits are added at the end of the DynamicBitSet (to the right).
     *
     * @param size The additional size (A size of 1 increase the DynamicBitSet of 64 bits).
     * @return The new capacity of the DynamicBitSet.
     */
    size_t increase(size_t size);

    /**
     * @brief Decrease the capacity of the DynamicBitSet by `64 * size` bits.
     *
     * @warning The bits removed will be lost forever.
     * @note The old bits are removed from the end of the DynamicBitSet (from the right).
     *
     * @param size The removed size (A size of 1 decrease the DynamicBitSet of 64 bits).
     * @return The new capacity of the DynamicBitSet.
     */
    size_t decrease(size_t size);

    /**
     * @brief Get the current capacity of the DynamicBitSet.
     *
     * @return The current capacity of the DynamicBitSet.
     */
    [[nodiscard]]
    size_t capacity() const;

    /** @return `true` if any bit is set. */
    [[nodiscard]]
    bool any() const;
    /** @return true if all tracked bits are set. */
    [[nodiscard]]
    bool all() const;
    /** @return true if no bits are set. */
    [[nodiscard]]
    bool none() const;

    /** @brief Clear all bits and release allocated blocks. */
    void clear();

    /**
     * @brief Bitwise AND
     * @return A new bitset.
     */
    DynamicBitSet operator&(const DynamicBitSet &other) const;
    /**
     * @brief Bitwise AND
     * @return The instance itself.
     */
    DynamicBitSet &operator&=(const DynamicBitSet &other);

    /**
     * @brief Bitwise OR
     * @return A new bitset.
     */
    DynamicBitSet operator|(const DynamicBitSet &other) const;
    /**
     * @brief Bitwise OR
     * @return The instance itself.
     */
    DynamicBitSet &operator|=(const DynamicBitSet &other);

    /**
     * @brief Bitwise XOR
     * @return A new bitset.
     */
    DynamicBitSet operator^(const DynamicBitSet &other) const;
    /**
     * @brief Apply a bitwise XOR.
     * @return The instance itself.
     */
    DynamicBitSet &operator^=(const DynamicBitSet &other);

    /**
     * @brief Bitwise NOT
     *
     * @return A new bitset.
     */
    DynamicBitSet operator~() const;

    /**
     * @brief Mutable access to bit at index `i`.
     *
     * @param i The index of the bit.
     * @return A mutable reference to the bit encapsulated in a DynamicBitSet::BitRef.
     */
    BitRef operator[](size_t i);
    /**
     * @brief Read-only access to bit at index `i`.
     *
     * @param i The index of the bit.
     * @return A boolean that represent the bit.
     */
    bool operator[](size_t i) const;

    /**
     * @brief Apply a left-bitshift on a DynamicBitSet copy.
     *
     * @param nb The number of bitshift to apply.
     * @return A copy of the DynamicBitSet.
     */
    DynamicBitSet operator<<(size_t nb) const;

    /**
     * @brief Apply a left-bitshift.
     *
     * @param nb The number of bitshift to apply.
     * @return A reference to the DynamicBitSet.
     */
    DynamicBitSet &operator<<=(size_t nb);

    /**
     * @brief Apply a right-bitshift on a DynamicBitSet copy.
     *
     * @param nb The number of bitshift to apply.
     * @return A copy of the DynamicBitSet.
     */
    DynamicBitSet operator>>(size_t nb) const;

    /**
     * @brief Apply a right-bitshift.
     *
     * @param nb The number of bitshift to apply.
     * @return A reference to the DynamicBitSet.
     */
    DynamicBitSet &operator>>=(size_t nb);

    /**
     * @brief Compare two bitsets for equality.
     *
     * @param other The other DynamicBitSet instance to compare.
     * @return `true` if the instances bitsets are equals, `false` otherwise.
     */
    bool operator==(const DynamicBitSet &other) const;
};

std::ostream &operator<<(std::ostream &stream,
                         const DynamicBitSet &ref);

}  // namespace rtecs::bitset
