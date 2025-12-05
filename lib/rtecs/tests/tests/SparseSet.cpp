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

    const rtecs::OptionalRef<MyComponent> component = sparseSet.getComponent(1);

    ASSERT_FALSE(sparseSet.has(0));
    ASSERT_FALSE(component.has_value());
}

TEST(SparseSet_create, create_multiple_entities) {
    struct MyComponent {
        std::string name;
        int age;
    };
    rtecs::SparseSet<MyComponent> sparseSet;

    for (int id = 1; id < 10; id++) {
        sparseSet.create(id);
    }
    for (int id = 1; id < 10; id++) {
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

    srand(time(nullptr));
    // Limits the number of page up to 64.
    constexpr size_t maximumId = rtecs::SparseSet<MyComponent>::kPageSize * 64;
    std::vector<size_t> selectedIds;
    rtecs::SparseSet<MyComponent> sparseSet;

    for (int i = 0; i < 10; i++) {
        const rtecs::EntityID id = rand() % maximumId;
        selectedIds.push_back(id);
        sparseSet.create(id);
    }
    for (const auto id : selectedIds) {
        const rtecs::OptionalRef<MyComponent> component = sparseSet.getComponent(id);
        ASSERT_TRUE(sparseSet.has(id));
        ASSERT_TRUE(component.has_value());
    }
}
