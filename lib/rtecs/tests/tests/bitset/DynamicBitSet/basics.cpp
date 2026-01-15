#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "rtecs/bitset/DynamicBitSet.hpp"

using namespace rtecs::bitset;

TEST(DynamicBitSet,
     serialize)
{
    const DynamicBitSet set{{
        std::bitset<64>{0b1100000000000000000000000000000000000000000000000000000000000001},
        std::bitset<64>{0b1100000000000000000000000000000000000000000000000000000000000001},
    }};
    const std::vector<uint64_t> expected{{0, 1, 63, 64, 65, 127}};

    ASSERT_EQ(set.serialize(), expected);
};

TEST(DynamicBitSet,
     deserialize)
{
    const DynamicBitSet set{{
        std::bitset<64>{0b111},
        std::bitset<64>{0b1},
    }};
    const DynamicBitSet deserialized = DynamicBitSet::deserialize(set.serialize());

    ASSERT_EQ(set, deserialized);
};

TEST(DynamicBitSet,
     empty_to_string)
{
    const DynamicBitSet set{};

    ASSERT_STREQ(set.toString().c_str(), "");
};

TEST(DynamicBitSet,
     filled_to_string)
{
    const DynamicBitSet set{{
        std::bitset<64>{0b1111111111111111111111111111111111111111111111111111111111111110},
        std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000000},
    }};

    ASSERT_STREQ(set.toString(" ").c_str(),
                 "1111111111111111111111111111111111111111111111111111111111111110 "
                 "1000000000000000000000000000000000000000000000000000000000000000");
};

TEST(DynamicBitSet,
     empty_serialize)
{
    const std::vector<uint64_t> expectedSerialized{};

    const DynamicBitSet set{{
        std::bitset<64>{0b0000000000000000000000000000000000000000000000000000000000000000},
        std::bitset<64>{0b0000000000000000000000000000000000000000000000000000000000000000},
    }};
    const std::vector<uint64_t> serializedSet = set.serialize();

    ASSERT_EQ(serializedSet.size(), expectedSerialized.size());
};

TEST(DynamicBitSet,
     filled_serialize)
{
    const std::vector<uint64_t> expectedSerialized{0, 61, 62, 63, 64, 65, 127};

    const DynamicBitSet set{{
        std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000111},
        std::bitset<64>{0b1100000000000000000000000000000000000000000000000000000000000001},
    }};
    const std::vector<uint64_t> serializedSet = set.serialize();

    ASSERT_EQ(serializedSet.size(), expectedSerialized.size());
    for (size_t i = 0; i < expectedSerialized.size(); i++) {
        EXPECT_EQ(serializedSet[i], expectedSerialized[i]);
    }
};

TEST(DynamicBitSet,
     bit_order_access)
{
    DynamicBitSet set{{
        std::bitset<64>{0b1110000000000000000000000000000000000000000000000000000000000001},
        std::bitset<64>{0b0100000000000000000000000000000000000000000000000000000000000101},
    }};

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

TEST(DynamicBitSet,
     bit_order_access_on_const_instance)
{
    const DynamicBitSet set{{
        std::bitset<64>{0b1110000000000000000000000000000000000000000000000000000000000001},
        std::bitset<64>{0b0100000000000000000000000000000000000000000000000000000000000101},
    }};

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

TEST(DynamicBitSet,
     bitset_capacity)
{
    DynamicBitSet set{{
        std::bitset<64>{0b0000000000000000000000000000000000000000000000000000000000000000},
    }};
    EXPECT_EQ(set.capacity(), 64);
    set[64] = true;
    EXPECT_EQ(set.capacity(), 128);
};

TEST(DynamicBitSet,
     bitsets_comparison)
{
    const DynamicBitSet doubled{{
        std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000001},
        std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000001},
    }};

    const DynamicBitSet lastEmpty{{
        std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000001},
        std::bitset<64>{0b0000000000000000000000000000000000000000000000000000000000000000},
    }};

    const DynamicBitSet single{{
        std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000001},
    }};

    EXPECT_FALSE(doubled == single);
    EXPECT_FALSE(single == doubled);

    EXPECT_FALSE(doubled == lastEmpty);
    EXPECT_FALSE(lastEmpty == doubled);

    EXPECT_TRUE(single == lastEmpty);
    EXPECT_TRUE(lastEmpty == single);
};

TEST(DynamicBitSet,
     clear_bitset)
{
    DynamicBitSet set{{
        std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000001},
        std::bitset<64>{0b1000000000000000000000000000000000000000000000000000000000000001},
    }};

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

TEST(DynamicBitSet,
     check_for_bits_on_empty_bitset)
{
    const DynamicBitSet set{{
        std::bitset<64>{0b0000000000000000000000000000000000000000000000000000000000000000},
        std::bitset<64>{0b0000000000000000000000000000000000000000000000000000000000000000},
    }};

    EXPECT_FALSE(set.any());
    EXPECT_FALSE(set.all());
    EXPECT_TRUE(set.none());
};

TEST(DynamicBitSet,
     check_for_bits_on_filled_bitset)
{
    const DynamicBitSet set{{
        std::bitset<64>{0b1000000000001000000000000000000000000000000000000000000000000001},
        std::bitset<64>{0b1000000000000010000000000000000000000000000000000000000000000001},
    }};

    EXPECT_TRUE(set.any());
    EXPECT_FALSE(set.all());
    EXPECT_FALSE(set.none());
};

TEST(DynamicBitSet,
     check_for_bits_on_full_bitset)
{
    const DynamicBitSet set{{
        std::bitset<64>{0b1111111111111111111111111111111111111111111111111111111111111111},
        std::bitset<64>{0b1111111111111111111111111111111111111111111111111111111111111111},
    }};

    EXPECT_TRUE(set.any());
    EXPECT_TRUE(set.all());
    EXPECT_FALSE(set.none());
};
