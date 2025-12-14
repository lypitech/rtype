# `rtecs`

`rtecs` is a library that implement an optimised Entity Component System for C++.

## Features
- Register entities and its components using a bitmask.
- Register components and systems.
- Apply all systems
- Apply specific system from its ID.

## Installation

### Requirements

- C++ Compiler that supports C++23 (Clang 10+, GCC 10+, MSVC 19.28+)
- [CMake](https://cmake.org) version 3.20 or higher
- [Conan](https://conan.io) package manager

### Using the library in your project

Since `rtecs` is a library, you can include it in your project via CMake.

```cmake
add_subdirectory(<rtecs_dir> rtecs)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        rtecs
)
```

## How to use

**Create a new ECS:**
```c++
struct Component1 {
    int a;
    char b;
}

struct Component2 {
    uint64_t c;
    short int d;
}

rtecs::ECS ecs = rtecs::ECS::createWithComponents<Component1, Component2>();
```

**Register entity:**
```c++
struct Component1 { 
    int a;
    char b;
}
struct Component2 { /* ... */ }

rtecs::ECS ecs = /* ... */

rtecs::EntityID id = ecs.registerEntity<Component1>({ .a = 10, .b = 20 });
```

**Register system:**
```c++
class MySystem : public ASystem {
    explicit MySystem(rtecs::DynamicBitSet bitset)
    : ASystem(std::move(bitset))
    {
    }
}

struct MyComponent { /* ... */ }

rtecs::ECS ecs = /* ... */
const auto bitset = ecs->getComponentsBitSet<MyComponent>();
ecs->registerSystem(std::make_unique<MySystem>(bitset));
```

**Apply all registered systems:**
```c++
rtecs::ECS ecs = /* ... */

/* Component & System registration... */

ecs.applyAllSystems();
```

> [!IMPORTANT]
> The order in which the systems are called is the same as the systems have been registered (First registered, first called).
