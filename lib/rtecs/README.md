# `rtecs`

`rtecs` is a library that implement an optimized Entity Component System for 
C++.

It provides a flexible architecture to decouple data (Components) from logic 
(Systems), allowing for high-performance game development.

## Features
- **Sparse set storage:** High-performance component storage ensuring data locality 
  and O(1) lookups.
- **Dynamic bitsets:** Efficient bitmasking to handle entity-component associations 
  dynamically.
- **Flexible systems:** Register and run logic systems globally or individually by 
  ID.
- **Group views:** Create SparseGroups to iterate efficiently over entities 
  possessing specific subsets of components.
- **Safe architecture:** Automatic validation of entity existence and component 
  integrity.

## Compatibility

|        |                    macOS (AppleClang)                    |                Linux (G++)                |                      Windows (MSVC)                     |
|-------:|:--------------------------------------------------------:|:-----------------------------------------:|:-------------------------------------------------------:|
|  arm64 | ✅<br/>- `AppleClang 17.0.0.17000603`<br/>- `CMake 4.1.2` |                    ☑️                     |                            ☑️                           |
| x86_64 |                            ☑️                            | ✅<br/>- `GNU 15.2.0`<br/>- `CMake 3.31.6` | ✅<br/>- `MSVC 19.50.35718.0`<br/>- `CMake 4.11.1-msvc1` |

✅: Tested on real hardware  
☑️: Compiled but not physically tested

The indicated versions are 100% functional. Any older version MIGHT NOT work.

## Installation

### Requirements

- C++ Compiler that supports C++23 (Clang 10+, GCC 10+, MSVC 19.28+)
- [CMake](https://cmake.org) version 3.20 or higher
- [Conan](https://conan.io) package manager version 2.22.2

### Using the library in your project

Since `rtecs` is a library, you can include it in your project via CMake.

```cmake
add_subdirectory(<rtecs_dir> rtecs)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        rtecs
)
```

### Building tests

`rtecs` comes with a suite of unit tests (that uses
[GTest](https://github.com/google/googletest)).  
You can build them by following these steps:

1. Fetch dependencies with Conan
```sh
conan install . --output-folder=build/ --build=missing -s build_type=Debug
```

2. Configure the project
```sh
cmake -S . -B build/ \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DRECS_BUILD_TESTS=ON
```

3. Build the library
```sh
cmake --build build/ # --parallel for faster compilation
```

4. Run the unit tests suite
```sh
ctest --test-dir build/ --output-on-failure
```

## How to use

### Summary

[1. Components](#components)<br>
[2. Systems](#systems)<br>
[3. Entities](#entities)<br>

----

### Components

A component is a structure that will contain data. Its role is to store data of a single entity.

**Component creation**

This example shows how to properly define and use a component.
```c++
struct Health
{
    int hp;
}

struct Profile
{
    const std::string name;
}

struct Arrow
{
    int[2] direction;
}

struct Transformation2D
{
    int x;
    int y;
    int[2] scale;
    int[2] rotation;
}

struct CollideBox2D
{
    int top;
    int left;
    int width;
    int height;
}
```

**Register a component**
```c++
#include "rtecs/ECS.hpp"

rtecs::ECS ecs;

// You can register multiple components on a single method call
ecs.registerComponents<Health, Profile, Arrow, Transformation2D, CollideBox2D>();

// Or register only one component by one
ecs.registerComponents<Health>();
ecs.registerComponents<Profile>();
ecs.registerComponents<Arrow>();
ecs.registerComponents<Transformation2D>();
ecs.registerComponents<CollideBox2D>();
```

**Get the mask corresponding to multiple components**
```c++
ecs.getComponentMask<Transformation2D, Health>();
```

> [!TIP]
> To send a mask through network, there is a `DynamicBitSet::serialize` method that
> returns a `std::vector` of all the enabled bits. You can use this method to send the mask through the network.

**Group creation and manipulation**
```c++
rtecs::sparse::SparseGroup<Transformation2D, Health, Profile> group = ecs.group<Transformation2D, Health, Profile>

// Manipulate group's instances
group.apply([](rtecs::types::EntityID entityId, Transformation2D& transformation, Health& health, Profile& profile) {
    transformation.x += 20;
    health.hp -= 1;
    LOG_TRACE_R3("Profile : {}", profile.name);
})

// Get a single component instance
rtecs::types::EntityID entityId = 0;
rtecs::types::OptionalRef<Profile> optionalProfile = group.getEntity<Profile>(entityId);

if (optionalProfile.has_value()) {
    Profile& profile = optionalProfile.value();
    LOG_TRACE_R3("Profile : {}", profile.name);
} else {
    LOG_WARNING("Profile not found...");
}

// Get all the component instances
auto& view = group.getAllInstances<Profile>(); // This view will contain all the instances of the Profile component

// Check if an entity is present in the group
if (group.has(entityId)) {
    LOG_TRACE_R3("Entity {} is present in the group", entityId);
} else {
    LOG_WARNING("Entity {} not found...", entityId);
}
```

----

### Systems

A system is a function that will be called at each call of the `ECS::applyAllSystems()` method. Its role is to manipulate components.

**Implement a system**
```c++
#include "rtecs/systems/ASystem.hpp"

class DamageOnArrowCollision : public systems::ASystem
{
public:
    explicit DamageOnArrowCollision():
        ASystem("DamageOnArrowCollision") {} // The name of the system will be used for debugging.

    void apply(ECS& ecs) override
    {
        // Retrieve all entities that have at least the Profile component and the CollideBox2D component
        sparse::SparseGroup<Health, CollideBox2D> players = ecs.group<Health, CollideBox2D>();
        
        // Retrieve all entities that have at least the Arrow component and the CollideBox2D component
        sparse::SparseGroup<Arrow, CollideBox2D> arrows = ecs.group<Arrow, CollideBox2D>();
        
        players.apply([&](rtecs::types::EntityID, Health& playerHealth, const CollideBox2D& playerBox) {
            arrows.apply([&playerBox](rtecs::types::EntityID, const Arrow&, const CollideBox2D& arrowBox) {
                if (/* Check for collision */) {
                    playerHealth.hp -= 1;
                    // Don't kill the player here, create a system that will play an animation if the player's health is lower than 0 !
                }
            }); // arrows.apply
        }); // players.apply
    }
};
```

**Register a system**
```c++
#include "rtecs/ECS.hpp"

rtecs::ECS ecs;

// This is the proper way to register a system
ecs.registerSystem(std::make_shared<DamageOnArrowCollision>());

// It is also possible to register a system from a lambda
ecs.registerSystem([](ECS &ecs) {
    // Your implementation of the ASystem::apply method goes here...
});

// DO NOT REGISTER SYSTEMS LIKE THAT
auto system = std::make_shared<DamageOnArrowCollision>();
ecs.registerSystem(std::move(system));
```

**Apply systems**
```c++
ecs.applyAllSystems();
```

> [!IMPORTANT]
> The order in which the systems are called is the same as the order of registration: First registered, first called.

---

### Entities

An entity is represented by a number to which we will associate multiple components.

> [!WARNING]
> Some methods will log a warning if any problem concerning an invalid entity occurs.

**Register an entity**

To register an entity, you will have to specify its components and a default value for each component.
```c++
#include "rtecs/ECS.hpp"

rtecs::ECS ecs;

/* Register your components and your systems first... */

// Specify all the components the entity have
rtecs::types::EntityID entityId = ecs.registerEntity<Profile, Health, CollideBox2D>(
    { "L1x" },                      // Profile
    { 20 },                         // Health
    { 0, 0, 100, 300 },             // CollideBox2D
    { 0, 0, { 1, 1 }, { 0, 0 } }    // Transformation2D
);
```

**Add components to an entity**
```c++
// If you need to add a component later after the entity registration, you can do it easily
ecs.addEntityComponents<Transformation2D>(
    entityId,                    // The ID of the registered Entity
    { 0, 0, { 1, 1 }, { 0, 0 } } // Transformation2D
);
```

**Update an entity component instance**
```c++
// Update an entity component outside of a system
ecs.updateEntityComponent<Health>(entityId, { 25 });
```

**Destroy an entity**
```c++
// Destroy an entity
ecs.destroyEntity(entityId);
```

> [!IMPORTANT]
> A destroyed entity can still be present in a SparseGroup, but using it will produce a memory error. This is why you should never store a SparseGroup anywhere.

**Get the component mask of an entity**
```c++
// Get the mask of an entity
const rtecs::bitset::DynamicBitSet& mask = ecs.getEntityMask(entityId);
```

> [!TIP]
> To send a mask through network, there is a `DynamicBitSet::serialize` method that
> returns a `std::vector` of all the enabled bits. You can use this method to send the mask through the network.
