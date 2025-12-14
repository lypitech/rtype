# `rtnt`

`rtnt` is a C++ cross-platform network library built on top of [Asio](https://think-async.com/Asio/) (standalone,
non-boost).  
It provides an abstraction over [UDP](https://en.wikipedia.org/wiki/User_Datagram_Protocol) and is designed for
real-time applications like games or low-latency software.

It features a high-level packet system, automatic endianness handling, and a callback-based dispatcher, allowing
developers to focus on game logic rather than raw socket management.

> [!NOTE]
> This library is currently in version **1.1.0**.  
> Full RUDP support will be introduced in version **1.2.0**.  
> Security layer (packet encryption and certificates) will be introduced in version **2.0.0**.  
> You can see the changelogs in the folder `changelogs`.

## Features
- Simple API: Client/Server architecture with an easy-to-use event loop.
- Safety: Automatic header validation, protocol ID checks, and size verification to reject noise and
corrupted/duplicate packets.
- Asynchronous core: Powered by `asio::io_context` for high-performance non-blocking IO.

## Compatibility

|        | macOS (AppleClang) | Linux (Clang) | Windows (MSVC) |
|-------:|:------------------:|:-------------:|:--------------:|
|  arm64 |         ✅          |      ☑️       |       ☑️       |
| x86_64 |         ☑️         |       ✅       |       ✅        |

✅: Tested on real hardware  
☑️: Compiled but not deeply tested

## Installation

### Requirements

- C++ Compiler that supports C++23 (Clang 10+, GCC 10+, MSVC 19.28+)
- CMake version 3.20 or higher
- Conan package manager

### Using the library in your project

Since `rtnt` is a library, you can include it in your project via CMake.

```cmake
add_subdirectory(<rtnt_dir> rtnt)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        rtnt
)
```

Keep in mind that `rtnt` uses [Shuvlog](https://github.com/lypitech/shuvlog) as its logging engine.  
Please read Shuvlog's documentation to know how to properly setup `rtnt`'s logs.

### Building tests

`rtnt` comes with a suite of unit tests. You can build them by following these steps:

1. Fetch dependencies with Conan
```sh
conan install . --output-folder=build/ --build=missing -s build_type=Debug
```

2. Configure the project
```sh
cmake -S . -B build/ \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DRTNT_BUILD_TESTS=ON
```

3. Build the library
```sh
cmake --build build/ # --parallel for faster compilation
```

4. Run the unit tests suite
```she
ctest --test-dir build/ --output-on-failure
```

| Test name     | Status | Notes |
|---------------|:------:|-------|
| Handshake     |   ✅    |       |
| Disconnect    |   ✅    |       |
| Empty packet  |   ✅    |       |
| String packet |   ✅    |       |
| Broadcast     |   ✅    |       |


## How to use

### 1. Packets

Packets are simple C++ structs. You must define a `kId` (unique identifier) and a `kName`.  
Though `kName` is not mandatory, it is strongly recommended to define it. In all cases, a warning will be logged if
`kName` is missing.

> [!CAUTION]
> User-defined packet IDs must be in the range `128-65535`. `rtnt` reserves the first 127 IDs for its internal
> packets.  
> Giving an ID that is in the internal range (0-127) is considered a violation, triggering a compiler assertion error.

Here is a packet implementation example:
```c++
#include <rtnt/core/packet.hpp>

struct Example
{
    static constexpr rtnt::core::packet::Id   kId   = 1801;      ///< Unique identifier of the packet.
                                                                 ///  MUST be in the range 128-65535!
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE"; ///< Name that will be used in the logs.
                                                                 ///  Won't be transmitted through the network!

    std::string sampleString;
    int sampleInt;

    // The serialize function defines how to write or read data.
    template <typename Archive>
    void serialize(Archive& ar) {
        ar & sampleString & sampleInt;
    }
};
```

#### 1.1 Packet serialization

Whenever a packet structure contains a custom body, it must tell `rtnt` "you have to write data on the network in THAT
order".

To do that, you have to implement the `serialize` function, with all the fields inside it:
```c++
// Inside the packet structure
template <typename Archive>
void serialize(Archive& ar) {
    ar & field1 & field2 & field3...;
}
```

> [!CAUTION]
> Not providing a `serialize` function in a packet that contains a custom body will make it sterile (unsendable).  
> If you try to do so, a compiler error will be thrown.

For now, fields can either be a primitive type (`int`, `floats`, basic `struct`s) or a `std::string`. Anything else will
result in undefined behavior.

#### 1.2
