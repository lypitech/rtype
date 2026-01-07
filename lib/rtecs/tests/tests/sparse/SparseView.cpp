#include "rtecs/sparse/view/SparseView.hpp"

#include <gtest/gtest.h>

#include "rtecs/sparse/set/SparseSet.hpp"

using namespace rtecs::sparse;

TEST(SparseVectorView, put_multiple_values)
{
    SparseView<size_t, size_t> view;

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
    SparseView<size_t, size_t> view;

    view.erase(2);
    ASSERT_FALSE(view.has(2));
}

TEST(SparseVectorView, erase_present_value)
{
    SparseView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 4);
    view.put(3, 6);
    view.put(4, 8);

    view.erase(2);
    ASSERT_EQ(view[1], 2);
    ASSERT_FALSE(view.has(2));
    ASSERT_EQ(view[3], 6);
    ASSERT_EQ(view[4], 8);
}

TEST(SparseVectorView, has_value)
{
    SparseView<size_t, size_t> view;

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
    SparseView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 2);
    view.put(7, 1);
    ASSERT_EQ(view[1], 2);
    ASSERT_EQ(view[2], 2);
    ASSERT_EQ(view[7], 1);
}

TEST(SparseVectorView, access_present_data_on_const)
{
    using TestSparseVectorView = SparseView<size_t, size_t>;
    TestSparseVectorView view;

    view.put(42, 84);
    ASSERT_TRUE(view.has(42));

    const std::function<void(const TestSparseVectorView &)> accessConst = [](const TestSparseVectorView &constView) {
        ASSERT_TRUE(constView.has(42));
        ASSERT_EQ(constView[42], 84);
    };
    accessConst(view);
}
