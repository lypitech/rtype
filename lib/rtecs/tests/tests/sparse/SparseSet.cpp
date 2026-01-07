#include "../../../include/rtecs/sparse/set/SparseSet.hpp"

#include <gtest/gtest.h>

#include "logger/Logger.h"

TEST(SparseSet, create_single_entity_without_auto_initializer)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };

    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);
    sparseSet.put(1, {.name = std::string("Hello world"), .age = 42});

    const rtecs::sparse::OptionalRef<MyComponent> first = sparseSet.get(1);

    ASSERT_TRUE(first.has_value());
}

TEST(SparseSet, create_single_entity_with_auto_initializer)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };

    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);
    sparseSet.put(1);

    const rtecs::sparse::OptionalRef<MyComponent> component = sparseSet.get(1);

    ASSERT_TRUE(component.has_value());
}

TEST(SparseSet, create_entity_with_id_zero)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };

    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);
    sparseSet.put(0, {.name = std::string("Hello world"), .age = 42});

    const rtecs::sparse::OptionalRef<MyComponent> component = sparseSet.get(0);

    ASSERT_TRUE(sparseSet.has(0));
    ASSERT_TRUE(component.has_value());
}

TEST(SparseSet, create_multiple_ordered_entities)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };
    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);

    for (int id = 0; id < 10; id++) {
        sparseSet.put(id);
    }
    for (int id = 0; id < 10; id++) {
        const rtecs::sparse::OptionalRef<MyComponent> component = sparseSet.get(id);
        ASSERT_TRUE(sparseSet.has(id));
        ASSERT_TRUE(component.has_value());
    }
}

TEST(SparseSet, create_multiple_entities_with_random_id)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };

    const std::vector<size_t> entities{1, 20, 3400, 4297, 9821, 12023};
    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);

    for (const size_t id : entities) {
        sparseSet.put(id);
    }
    for (const auto id : entities) {
        const rtecs::sparse::OptionalRef<MyComponent> component = sparseSet.get(id);
        ASSERT_TRUE(sparseSet.has(id));
        ASSERT_TRUE(component.has_value());
    }
}

TEST(SparseSet, clear_empty_sparseset)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };

    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);
    sparseSet.clear();

    ASSERT_FALSE(sparseSet.has(0));
}

TEST(SparseSet, clear_filled_sparseset)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };

    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);

    for (int id = 0; id < 10; id++) {
        sparseSet.put(id);
    }

    for (int id = 0; id < 10; id++) {
        EXPECT_TRUE(sparseSet.has(id));
    }

    sparseSet.clear();

    for (int id = 0; id < 10; id++) {
        ASSERT_FALSE(sparseSet.has(id));
    }
}

TEST(SparseSet, remove_entity)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };

    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);

    for (int id = 0; id < 10; id++) {
        sparseSet.put(id);
    }

    for (int id = 0; id < 10; id++) {
        EXPECT_TRUE(sparseSet.has(id));
    }

    sparseSet.remove(2);

    for (int id = 0; id < 10; id++) {
        if (id == 2) {
            ASSERT_FALSE(sparseSet.has(id));
        } else {
            ASSERT_TRUE(sparseSet.has(id));
        }
    }
}

TEST(SparseSet, remove_undefined_entity)
{
    struct MyComponent
    {
        std::string name;
        int age;
    };

    rtecs::sparse::SparseSet<MyComponent> sparseSet(0);

    for (int id = 0; id < 10; id++) {
        sparseSet.put(id);
    }

    sparseSet.remove(2);
    ASSERT_FALSE(sparseSet.has(2));
}
