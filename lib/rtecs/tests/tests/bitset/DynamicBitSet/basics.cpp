#include "rtecs/bitset/DynamicBitSet.hpp"

#include <gtest/gtest.h>

#include "logger/Logger.h"

using namespace rtecs::bitset;

TEST(DynamicBitSet, empty_to_string)
{
    const DynamicBitSet set{};

    ASSERT_STREQ(
        set.toString().c_str(),
        ""
    );
};

TEST(DynamicBitSet, filled_to_string)
{
    const DynamicBitSet set{
        {
            std::bitset<64>{ 0b1111111111111111111111111111111111111111111111111111111111111110 },
            std::bitset<64>{ 0b1000000000000000000000000000000000000000000000000000000000000000 },
        }
    };

    ASSERT_STREQ(
        set.toString(" ").c_str(),
        "1111111111111111111111111111111111111111111111111111111111111110 1000000000000000000000000000000000000000000000000000000000000000"
    );
};

TEST(DynamicBitSet, empty_serialize)
{
    const std::pair<std::vector<uint64_t>, size_t> expectedSerialized{
        {  },
        0,
    };

    const DynamicBitSet set{
        {
            std::bitset<64>{ 0b0000000000000000000000000000000000000000000000000000000000000000 },
            std::bitset<64>{ 0b0000000000000000000000000000000000000000000000000000000000000000 },
        }
    };
    const std::pair<std::vector<uint64_t>, size_t> serializedSet = set.serialize();

    ASSERT_EQ(serializedSet.first.size(), expectedSerialized.first.size());
    EXPECT_EQ(serializedSet.second, expectedSerialized.second);
    for (size_t i = 0; i < expectedSerialized.first.size(); i++)
        EXPECT_EQ(serializedSet.first[i], expectedSerialized.first[i]);
};

TEST(DynamicBitSet, filled_serialize)
{
    const std::pair<std::vector<uint64_t>, size_t> expectedSerialized{
            { 0, 61, 62, 63, 64, 65, 127 },
            128,
        };

    const DynamicBitSet set{
            {
                std::bitset<64>{ 0b1000000000000000000000000000000000000000000000000000000000000111 },
                std::bitset<64>{ 0b1100000000000000000000000000000000000000000000000000000000000001 },
            }
    };
    const std::pair<std::vector<uint64_t>, size_t> serializedSet = set.serialize();

    ASSERT_EQ(serializedSet.first.size(), expectedSerialized.first.size());
    EXPECT_EQ(serializedSet.second, expectedSerialized.second);
    for (size_t i = 0; i < expectedSerialized.first.size(); i++)
        EXPECT_EQ(serializedSet.first[i], expectedSerialized.first[i]);
};

TEST(DynamicBitSet, bit_order_access)
{
    DynamicBitSet set{
        {
            std::bitset<64>{ 0b1110000000000000000000000000000000000000000000000000000000000001 },
            std::bitset<64>{ 0b0100000000000000000000000000000000000000000000000000000000000101 },
        }
    };

    EXPECT_TRUE(set[0] == true);
    EXPECT_TRUE(set[1] == true);
    EXPECT_TRUE(set[2] == true);

    EXPECT_TRUE(set[61] == false);
    EXPECT_TRUE(set[62] == false);
    EXPECT_TRUE(set[63] == true);

    EXPECT_TRUE(set[64] == false);
    EXPECT_TRUE(set[65] == true);
    EXPECT_TRUE(set[66] == false);

    EXPECT_TRUE(set[125] == true);
    EXPECT_TRUE(set[126] == false);
    EXPECT_TRUE(set[127] == true);
};

TEST(DynamicBitSet, bit_order_access_on_const_instance)
{
    const DynamicBitSet set{
        {
            std::bitset<64>{ 0b1110000000000000000000000000000000000000000000000000000000000001 },
            std::bitset<64>{ 0b0100000000000000000000000000000000000000000000000000000000000101 },
        }
    };

    EXPECT_TRUE(set[0] == true);
    EXPECT_TRUE(set[1] == true);
    EXPECT_TRUE(set[2] == true);

    EXPECT_TRUE(set[61] == false);
    EXPECT_TRUE(set[62] == false);
    EXPECT_TRUE(set[63] == true);

    EXPECT_TRUE(set[64] == false);
    EXPECT_TRUE(set[65] == true);
    EXPECT_TRUE(set[66] == false);

    EXPECT_TRUE(set[125] == true);
    EXPECT_TRUE(set[126] == false);
    EXPECT_TRUE(set[127] == true);
};

TEST(DynamicBitSet, bitset_capacity)
{
    DynamicBitSet set{
        {
            std::bitset<64>{ 0b0000000000000000000000000000000000000000000000000000000000000000 },
        }
    };

    EXPECT_EQ(set.capacity(), 64);

    set[64] = true;

    EXPECT_EQ(set.capacity(), 128);
};

TEST(DynamicBitSet, bitsets_comparison)
{
    const DynamicBitSet doubled{
        {
            std::bitset<64>{ 0b1000000000000000000000000000000000000000000000000000000000000001 },
            std::bitset<64>{ 0b1000000000000000000000000000000000000000000000000000000000000001 },
        }
    };

    const DynamicBitSet lastEmpty{
        {
            std::bitset<64>{ 0b1000000000000000000000000000000000000000000000000000000000000001 },
            std::bitset<64>{ 0b0000000000000000000000000000000000000000000000000000000000000000 },
        }
    };

    const DynamicBitSet single{
        {
            std::bitset<64>{ 0b1000000000000000000000000000000000000000000000000000000000000001 },
        }
    };

    EXPECT_FALSE(doubled == single);
    EXPECT_FALSE(single == doubled);

    EXPECT_FALSE(doubled == lastEmpty);
    EXPECT_FALSE(lastEmpty == doubled);

    EXPECT_TRUE(single == lastEmpty);
    EXPECT_TRUE(lastEmpty == single);
};

TEST(DynamicBitSet, clear_bitset)
{
    DynamicBitSet set{
        {
            std::bitset<64>{ 0b1000000000000000000000000000000000000000000000000000000000000001 },
            std::bitset<64>{ 0b1000000000000000000000000000000000000000000000000000000000000001 },
        }
    };

    EXPECT_EQ(set.capacity(), 128);
    EXPECT_TRUE(set[0] == true);
    EXPECT_TRUE(set[63] == true);
    EXPECT_TRUE(set[64] == true);
    EXPECT_TRUE(set[127] == true);

    set.clear();

    EXPECT_FALSE(set[0] == true);
    EXPECT_FALSE(set[63] == true);
    EXPECT_FALSE(set[64] == true);
    EXPECT_FALSE(set[127] == true);
};

TEST(DynamicBitSet, check_for_bits_on_empty_bitset)
{
    const DynamicBitSet set{
        {
            std::bitset<64>{ 0b0000000000000000000000000000000000000000000000000000000000000000 },
            std::bitset<64>{ 0b0000000000000000000000000000000000000000000000000000000000000000 },
        }
    };

    EXPECT_FALSE(set.any());
    EXPECT_FALSE(set.all());
    EXPECT_TRUE(set.none());
};

TEST(DynamicBitSet, check_for_bits_on_filled_bitset)
{
    const DynamicBitSet set{
        {
            std::bitset<64>{ 0b1000000000001000000000000000000000000000000000000000000000000001 },
            std::bitset<64>{ 0b1000000000000010000000000000000000000000000000000000000000000001 },
        }
    };

    EXPECT_TRUE(set.any());
    EXPECT_FALSE(set.all());
    EXPECT_FALSE(set.none());
};

TEST(DynamicBitSet, check_for_bits_on_full_bitset)
{
    const DynamicBitSet set{
        {
            std::bitset<64>{ 0b1111111111111111111111111111111111111111111111111111111111111111 },
            std::bitset<64>{ 0b1111111111111111111111111111111111111111111111111111111111111111 },
        }
    };

    EXPECT_TRUE(set.any());
    EXPECT_TRUE(set.all());
    EXPECT_FALSE(set.none());
};
