#include <gtest/gtest.h>

#include "logger/Logger.h"
#include "rtecs/rtecs.h"

TEST(SparseSet_create, create_single_entity_without_auto_initializer) {
    struct MyComponent {
        std::string name;
        int age;
    };

    rtecs::SparseSet<MyComponent> sparseSet;
    sparseSet.create(1, { .name = std::string("Hello world"), .age = 42 });

    const rtecs::OptionalRef<MyComponent> first = sparseSet.getComponent(1);

    ASSERT_TRUE(first.has_value());
}

TEST(SparseSet_create, create_single_entity_with_auto_initializer) {
    struct MyComponent {
        std::string name;
        int age;
    };

    rtecs::SparseSet<MyComponent> sparseSet;
    sparseSet.create(1);

    const rtecs::OptionalRef<MyComponent> component = sparseSet.getComponent(1);

    ASSERT_TRUE(component.has_value());
}

TEST(SparseSet_create, create_entity_with_id_zero) {
    struct MyComponent {
        std::string name;
        int age;
    };

    rtecs::SparseSet<MyComponent> sparseSet;
    sparseSet.create(0, { .name = std::string("Hello world"), .age = 42 });

    const rtecs::OptionalRef<MyComponent> component = sparseSet.getComponent(0);

    ASSERT_TRUE(sparseSet.has(0));
    ASSERT_TRUE(component.has_value());
}

TEST(SparseSet_create, create_multiple_ordered_entities) {
    struct MyComponent {
        std::string name;
        int age;
    };
    rtecs::SparseSet<MyComponent> sparseSet;

    for (int id = 0; id < 10; id++) {
        sparseSet.create(id);
    }
    for (int id = 0; id < 10; id++) {
        const rtecs::OptionalRef<MyComponent> component = sparseSet.getComponent(id);
        ASSERT_TRUE(sparseSet.has(id));
        ASSERT_TRUE(component.has_value());
    }
}

TEST(SparseSet_create, create_multiple_entities_with_random_id) {
    struct MyComponent {
        std::string name;
        int age;
    };

    std::vector<size_t> entities{ 1, 20, 3400, 4297, 9821, 12023 };
    rtecs::SparseSet<MyComponent> sparseSet;

    for (const size_t id : entities) {
        sparseSet.create(id);
    }
    for (const auto id : entities) {
        const rtecs::OptionalRef<MyComponent> component = sparseSet.getComponent(id);
        ASSERT_TRUE(sparseSet.has(id));
        ASSERT_TRUE(component.has_value());
    }
}

TEST(SparseSet_clear, clear_empty_sparseset) {
    struct MyComponent {
        std::string name;
        int age;
    };

    rtecs::SparseSet<MyComponent> sparseSet;
    sparseSet.clear();

    ASSERT_FALSE(sparseSet.has(0));
}

TEST(SparseSet_clear, clear_filled_sparseset) {
    struct MyComponent {
        std::string name;
        int age;
    };

    rtecs::SparseSet<MyComponent> sparseSet;

    for (int id = 0; id < 10; id++) {
        sparseSet.create(id);
    }

    for (int id = 0; id < 10; id++) {
        EXPECT_TRUE(sparseSet.has(id));
    }

    sparseSet.clear();

    for (int id = 0; id < 10; id++) {
        ASSERT_FALSE(sparseSet.has(id));
    }
}

TEST(SparseSet_destroy, destroy_entity) {
    struct MyComponent {
        std::string name;
        int age;
    };

    rtecs::SparseSet<MyComponent> sparseSet;

    for (int id = 0; id < 10; id++) {
        sparseSet.create(id);
    }

    for (int id = 0; id < 10; id++) {
        EXPECT_TRUE(sparseSet.has(id));
    }

    sparseSet.destroy(2);
    ASSERT_FALSE(sparseSet.has(2));

    for (int id = 0; id < 2; id++) {
        ASSERT_TRUE(sparseSet.has(id));
    }
    for (int id = 3; id < 10; id++) {
        ASSERT_TRUE(sparseSet.has(id));
    }
}
