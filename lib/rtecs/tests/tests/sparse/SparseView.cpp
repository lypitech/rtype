#include "rtecs/sparse/view/SparseView.hpp"

#include <gtest/gtest.h>

#include "rtecs/sparse/set/SparseSet.hpp"

using namespace rtecs::sparse;

TEST(SparseView, put_multiple_values)
{
    SparseView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 4);
    view.put(3, 6);
    view.put(4, 8);

    ASSERT_TRUE(view.at(1).has_value());
    EXPECT_EQ(view.at(1).value(), 2);
    ASSERT_TRUE(view.at(2).has_value());
    EXPECT_EQ(view.at(2).value(), 4);
    ASSERT_TRUE(view.at(3).has_value());
    ASSERT_TRUE(view.at(3).has_value());
    EXPECT_EQ(view.at(3).value(), 6);
    ASSERT_TRUE(view.at(4).has_value());
    EXPECT_EQ(view.at(4).value(), 8);
}

TEST(SparseView, erase_unknown_value)
{
    SparseView<size_t, size_t> view;

    view.erase(2);
    ASSERT_FALSE(view.has(2));
}

TEST(SparseView, erase_present_value)
{
    SparseView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 4);
    view.put(3, 6);
    view.put(4, 8);

    view.erase(2);
    ASSERT_TRUE(view.at(1).has_value());
    ASSERT_FALSE(view.at(2).has_value());
    ASSERT_TRUE(view.at(3).has_value());
    ASSERT_TRUE(view.at(4).has_value());

    EXPECT_EQ(view.at(1).value(), 2);
    EXPECT_FALSE(view.has(2));
    EXPECT_EQ(view.at(3).value(), 6);
    EXPECT_EQ(view.at(4).value(), 8);
}

TEST(SparseView, has_value)
{
    SparseView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 4);
    view.put(3, 6);
    view.put(4, 8);

    ASSERT_TRUE(view.at(1).has_value());
    ASSERT_TRUE(view.at(2).has_value());
    ASSERT_TRUE(view.at(3).has_value());
    ASSERT_TRUE(view.at(4).has_value());

    EXPECT_EQ(view.at(1).value(), 2);
    EXPECT_EQ(view.at(2).value(), 4);
    EXPECT_EQ(view.at(3).value(), 6);
    EXPECT_EQ(view.at(4).value(), 8);

    EXPECT_TRUE(view.has(1));
    EXPECT_TRUE(view.has(2));
    EXPECT_TRUE(view.has(3));
    EXPECT_TRUE(view.has(4));
    EXPECT_FALSE(view.has(5));
    EXPECT_FALSE(view.has(6));
}

TEST(SparseView, access_present_data)
{
    SparseView<size_t, size_t> view;

    view.put(1, 2);
    view.put(2, 2);
    view.put(7, 1);

    ASSERT_TRUE(view.at(1).has_value());
    ASSERT_TRUE(view.at(2).has_value());
    ASSERT_TRUE(view.at(7).has_value());

    EXPECT_EQ(view.at(1).value(), 2);
    EXPECT_EQ(view.at(2).value(), 2);
    EXPECT_EQ(view.at(7).value(), 1);
}

TEST(SparseView, access_present_data_on_const)
{
    using TestSparseView = SparseView<size_t, size_t>;
    TestSparseView view;

    view.put(42, 84);
    EXPECT_TRUE(view.has(42));

    const std::function<void(const TestSparseView &)> accessConst = [](const TestSparseView &constView) {
        ASSERT_TRUE(constView.at(42).has_value());
        EXPECT_EQ(constView.at(42).value(), 84);
        EXPECT_TRUE(constView.has(42));
    };
    accessConst(view);
}
