# `rtnt`

`rtnt` is a C++ cross-platform network library built on top of [Asio](https://think-async.com/Asio/) (standalone,
non-boost).  
It provides an abstraction over [UDP](https://en.wikipedia.org/wiki/User_Datagram_Protocol) and is designed for
real-time applications like games or low-latency software.

It features a high-level packet system, automatic endianness handling, and a callback-based dispatcher, allowing
developers to focus on game logic rather than raw socket management.

> [!NOTE]
> This library is currently in version **1.1.1**. Things are prone to change!  
> Full [RUDP](https://en.wikipedia.org/wiki/Reliable_User_Datagram_Protocol) support will be introduced in version
> **1.2.0**.  
> Security layer (packet encryption and certificates) will be introduced in version **2.0.0**.  
> You can read the changelogs in the folder `changelogs`.

## Features
- **Simple API:** Client/Server architecture with an easy-to-use event loop.
- **Safety:** Automatic header validation, protocol ID checks, and size verification to reject noise and
corrupted/duplicate packets.
- **Asynchronous core:** Powered by `asio::io_context` for high-performance non-blocking IO.

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
- [CMake](https://cmake.org) version 3.20 or higher
- [Conan](https://conan.io) package manager

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
> Giving an ID that is in the internal range (0-127) is considered a violation, that will ultimately trigger a compiler
> assertion error.

Here is a packet implementation example:
```c++
#include "rtnt/core/packet.hpp"

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
    void serialize(Archive& ar)
    {
        ar & sampleString & sampleInt;
    }
};
```

#### 1.1 Packet serialization

Whenever a packet structure contains a custom body, it must tell `rtnt` "you have to write THESE data on the network
in THAT order".

To do that, you have to implement the `serialize` function, with all the fields inside it:
```c++
// Inside the packet structure
template <typename Archive>
void serialize(Archive& ar)
{
    ar & field1 & field2 & field3...;
}
```

> [!CAUTION]
> Not providing a `serialize` function in a packet that contains a custom body will make it sterile (unsendable).  
> If you try to do so, a compiler error will be thrown.
> ```c++
> /// No body, no need to implement the serialize() function
> struct Bodiless
> {
>     static constexpr rtnt::core::packet::Id kId = 1801;
>     static constexpr rtnt::core::packet::Name kName = "EXAMPLE";
> };
> 
> /// Has a body, MUST implement the serialize() function
> struct Bodied
> {
>     static constexpr rtnt::core::packet::Id kId = 1801;
>     static constexpr rtnt::core::packet::Name kName = "EXAMPLE";
> 
>     std::string str;
> 
>     template <typename Archive>
>     void serialize(Archive& ar)
>     {
>         ar & str;
>     }
> };
> 
> /// This packet structure just won't compile when put in a send or broadcast.
> struct WrongBodied
> {
>     static constexpr rtnt::core::packet::Id kId = 1801;
>     static constexpr rtnt::core::packet::Name kName = "EXAMPLE";
> 
>     std::string str;
> };
> ```

For now, fields can either be a primitive type (`int`, `floats`, basic `struct`), `std::string` or a `std::vector`
(containing supported types). Anything else will result in undefined behavior.

### 2. Server

Hosting a server is relatively simple. You just have to specify the port you want the server to be hosted on and
everything else will be handled automatically.

Whenever you're ready, call the `run()` function of both `Server` and `asio::io_context`.  
After that, don't forget to periodically update the server with the `update()` function.

You can set a callback for each session connection/disconnection, with the `onConnect` and `onDisconnect` functions.

Full example (**PRONE TO CHANGE**):
```c++
#include "rtnt/core/server.hpp"

int main()
{
    constexpr unsigned short port = 4242;
    constexpr size_t TPS = 20;  // Number of time the server will refresh its state per second.
                                // This doesn't affect the I/O operations, they will still run in real-time.
    
    asio::io_context ctx;
    std::thread ioThread;

    rtnt::core::Server server(ctx, port);

    server.onConnect([](const std::shared_ptr<rtnt::core::Session>& session) {
        LOG_INFO("New client connected! ID: {}", session->getId());
    });

    server.onDisconnect([](const std::shared_ptr<rtnt::core::Session>& session) {
        LOG_INFO("Client disconnected. ID: {}", session->getId());
    });

    server.start();

    ioThread = std::thread([this]() {
        logger::setThreadLabel("IoThread"); // Not MANDATORY but heavily recommended for easier log reading.
        ctx.run();
    });
    
    auto start = std::chrono::steady_clock::now();

    while (true) {
        server->update();   // This function can take quite some time, especially during heavy connections load.
                            // Consider implementing a dynamic sleep, will be done in v1.2.X.

        std::this_thread::sleep_for(std::chrono::milliseconds(1 / TPS * 1000));
    }

    ctx.stop();

    if (ioThread.joinable()) {
        ioThread.join();
    }
}
```

### 3. Client

Creating a client is similar to hosting a server. Simply call the `connect()` function with the remote server's IP and
port. Everything else is done under the hood.

The client will notify you when you are successfully connected with the remote server via the `onConnect` callback.  
The connection is considered successful whenever the remote server responds with the right packet.

Whenever you're ready, call the `run()` function of `asio::io_context`.  
After that, don't forget to periodically update the client with the `update()` function.

You can also set a callback for when the client is/gets disconnected, with the `onDisconnect` function.

Full example (**PRONE TO CHANGE**):
```c++
#include "rtnt/core/client.hpp"

int main()
{
    constexpr std::string_view ip = "127.0.0.1";
    constexpr unsigned short port = 4242;
    constexpr size_t TPS = 20;  // Number of time the client will refresh its state per second.
                                // This doesn't affect the I/O operations, they will still run in real-time.

    asio::io_context ctx;
    std::thread ioThread;

    rtnt::core::Client client(ctx);

    client.onConnect([]() {
        LOG_INFO("Client connected!");
    });

    client.onDisconnect([]() {
        LOG_INFO("Client disconnected.");
    });

    client.connect(ip, port);

    ioThread = std::thread([this]() {
        logger::setThreadLabel("IoThread"); // Not MANDATORY but heavily recommended for easier log reading.
        ctx.run();
    });
    
    auto start = std::chrono::steady_clock::now();

    while (true) {
        client->update();   // This function can take quite some time, especially during heavy connections load.
                            // Consider implementing a dynamic sleep, will be done in v1.2.X.

        std::this_thread::sleep_for(std::chrono::milliseconds(1 / TPS * 1000));
    }

    ctx.stop();

    if (ioThread.joinable()) {
        ioThread.join();
    }
}
```

### 4. Packet handling

> [!NOTE]
> As this system is still under development, it may be prone to change.

There are two ways of handling packets.

Either by having an integrated callback inside the packet structure:
```c++
struct Example
{
    static constexpr rtnt::core::packet::Id   kId   = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";

    static void onReceive(const std::shared_ptr<rtnt::core::Session>& session, const Example& packet)
    {
        // Your callback code goes here
    }
};

Dispatcher#bind<Example>(); // Automatically detects the onReceive function inside the packet structure.
```

...or by manually binding a packet type to a given callback (with a lambda function):
```c++
struct Example
{
    static constexpr rtnt::core::packet::Id   kId   = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";
};

Dispatcher#bind<Example>(
    [/* You can put whatever you want in the inject scope, like 'this' to inject the current class in the lambda */]
    (
        const std::shared_ptr<rtnt::core::Session>& session, 
        const Example& packet
    )
    {
        // Your callback code goes here
    }
);
```

The dispatcher can be fetched through the Server or the Client, depending on which you use.
```c++
Client#getPacketDispatcher();
Server#getPacketDispatcher();
///> Returns a reference to the internal Dispatcher.
```

## Protocol (`rtntp`)

The `rtnt` protocol (`rtntp`) operates over [UDP](https://en.wikipedia.org/wiki/User_Datagram_Protocol).  
Every packet is prefixed with a fixed-size header to ensure integrity and identify the message type.

**Header layout (packed)**

| Offset (hex) | Size (bytes) |         Field         | Description                                                                                                                                                   |
|:------------:|:------------:|:---------------------:|---------------------------------------------------------------------------------------------------------------------------------------------------------------|
|     0x00     |  2 (`u16`)   |     `protocolId`      | Magic number to reject random internet noise.                                                                                                                 |
|     0x02     |  2 (`u16`)   |   `protocolVersion`   | Protocol version.                                                                                                                                             |
|     0x04     |  4 (`u32`)   |     `sequenceId`      | Incremental ID for packet ordering/loss detection.                                                                                                            |
|     0x08     |  4 (`u32`)   |    `acknowledgeId`    | The last `sequenceId` received from the Peer (either Server or Client depending on the context).                                                              |
|     0x0C     |  4 (`u32`)   | `acknowledgeBitfield` | Bitmask representing the previous 32 packets received **(not implemented yet)**.                                                                              |
|     0x10     |  2 (`u16`)   |      `messageId`      | User-defined ID.                                                                                                                                              |
|     0x12     |   1 (`u8`)   |        `flags`        | Reliability flags (Unreliable, Reliable, Ordered) **(not implemented yet)**.                                                                                  |
|     0x13     |  2 (`u16`)   |     `payloadSize`     | Size of the user data following the header.                                                                                                                   |
|     0x15     |  4 (`u32`)   |      `checksum`       | [CRC32](https://en.wikipedia.org/wiki/Cyclic_redundancy_check) checksum to validate payload integrity and detect packet corruption **(not implemented yet)**. |

Header size: 25 bytes.

When receiving a packet, multiple checks are performed:
- **Header size:** If received data is too small to contain a `rtnt` header, packet is dropped, considered as random
  internet noise.
- **Protocol ID:** If received protocol ID doesn't match the local protocol ID, packet is dropped.
- **Payload size:** If given protocol size doesn't match the size of the payload received, packet is dropped, considered
  as corrupted.
