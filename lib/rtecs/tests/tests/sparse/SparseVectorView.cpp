#include "rtecs/sparse/SparseVectorView.hpp"

#include <gtest/gtest.h>

#include "rtecs/sparse/SparseSet.hpp"

using namespace rtecs::sparse;

TEST(SparseVectorView, put_multiple_values)
{
    SparseVectorView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 4);
    view.put(3, 6);
    view.put(4, 8);

    ASSERT_EQ(view[1], 2);
    ASSERT_EQ(view[2], 4);
    ASSERT_EQ(view[3], 6);
    ASSERT_EQ(view[4], 8);
}

TEST(SparseVectorView, erase_unknown_value)
{
    SparseVectorView<size_t, size_t> view;

    view.erase(2);
    ASSERT_EQ(view[2], 0);
}

TEST(SparseVectorView, erase_present_value)
{
    SparseVectorView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 4);
    view.put(3, 6);
    view.put(4, 8);

    view.erase(2);
    ASSERT_EQ(view[1], 2);
    ASSERT_EQ(view[2], 0);
    ASSERT_EQ(view[3], 6);
    ASSERT_EQ(view[4], 8);
}

TEST(SparseVectorView, has_value)
{
    SparseVectorView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 4);
    view.put(3, 6);
    view.put(4, 8);

    EXPECT_EQ(view[1], 2);
    EXPECT_EQ(view[2], 4);
    EXPECT_EQ(view[3], 6);
    EXPECT_EQ(view[4], 8);

    ASSERT_TRUE(view.has(1));
    ASSERT_TRUE(view.has(2));
    ASSERT_TRUE(view.has(3));
    ASSERT_TRUE(view.has(4));
    ASSERT_FALSE(view.has(5));
    ASSERT_FALSE(view.has(6));
}

TEST(SparseVectorView, access_present_data)
{
    SparseVectorView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 2);
    view.put(7, 1);
    ASSERT_EQ(view[1], 2);
    ASSERT_EQ(view[2], 2);
    ASSERT_EQ(view[7], 1);
}

TEST(SparseVectorView, access_absent_data)
{
    SparseVectorView<size_t, size_t> view;

    ASSERT_FALSE(view.has(1));
    ASSERT_FALSE(view.has(2));
    ASSERT_FALSE(view.has(6));

    ASSERT_EQ(view[1], 0);
    ASSERT_EQ(view[2], 0);
    ASSERT_EQ(view[6], 0);

    ASSERT_TRUE(view.has(1));
    ASSERT_TRUE(view.has(2));
    ASSERT_TRUE(view.has(6));
}

TEST(SparseVectorView, access_present_data_on_const)
{
    using TestSparseVectorView = SparseVectorView<size_t, size_t>;
    TestSparseVectorView view;

    view.put(42, 84);
    ASSERT_TRUE(view.has(42));

    const std::function<void(const TestSparseVectorView &)> accessConst = [](const TestSparseVectorView &constView) {
        ASSERT_TRUE(constView.has(42));
        ASSERT_EQ(constView[42], 84);
    };
    accessConst(view);
}
