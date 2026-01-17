# `rtnt`

`rtnt` is a C++ cross-platform network library built on top of [Asio](https://think-async.com/Asio/)
(standalone, non-boost).  
It provides an abstraction over [UDP](https://en.wikipedia.org/wiki/User_Datagram_Protocol) and is designed for real-time
applications like games or low-latency software.

It features a high-level packet system, automatic endianness handling, hybrid
reliability (acknowledgement) and a callback-based dispatcher, allowing
developers to focus on game logic rather than raw socket management.

> [!NOTE]
> This library is currently in version **1.4.1**. Things are prone to change!  
> Security layer (packet encryption and certificates) will be introduced in
> version **2.0.0**.

[//]: # (> You can read the changelogs in the folder `changelogs`.)

## Features
- **Simple API:** Client/Server architecture with an easy-to-use event loop.
- **Robust reliability:** Uses a hybrid acknowledgement system:
  - **Bitfield ACKs:** Standard ACKs (`__rtnt_internal_ACK`) are sent every 16
    packets (safety buffer) to keep the bandwidth low.
  - **Rich ACKs:** In high packet loss scenarios, Rich ACKs
    (`__rtnt_internal_RICH_ACK`) containing specific lists of missing packet IDs
    are sent to recover data without infinite retransmission loops.
- **Virtual channels:** Multiplexing logic allowing parallel ordering streams
  (for example chat is ordered, movement is unreliable).
- **Safety:** Automatic header validation, protocol ID checks,
  [MTU](https://fr.wikipedia.org/wiki/Maximum_transmission_unit)-safe
  fragmentation, and size verification to reject noise and corrupted/duplicate
  packets.
- **Asynchronous core:** Powered by `asio::io_context` for high-performance
  non-blocking IO.

## Compatibility

|        |       macOS (AppleClang)       | Linux (Clang) | Windows (MSVC) |
|-------:|:------------------------------:|:-------------:|:--------------:|
|  arm64 | ✅ (AppleClang 17.0.0.17000603) |      ☑️       |       ☑️       |
| x86_64 |               ☑️               |   ✅ (Clang)   |    ✅ (MSVC)    |

✅: Tested on real hardware  
☑️: Compiled but not physically tested

## Installation

### Requirements

- C++ Compiler that supports C++23 (Clang 10+, GCC 10+, MSVC 19.28+)
- [CMake](https://cmake.org) version 3.20 or higher
- [Conan](https://conan.io) package manager version 2.22.2 or higher

### Using the library in your project

Since `rtnt` is a library, you can include it in your project via CMake.

```cmake
add_subdirectory(<rtnt_dir> rtnt)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        rtnt
    PUBLIC
        asio::asio
)
```

Keep in mind that `rtnt` uses [Shuvlog](https://github.com/lypitech/shuvlog) as its logging engine.  
Please read Shuvlog's documentation to know how to properly setup `rtnt`'s logs.

### Building tests

`rtnt` comes with a suite of unit tests (that uses
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
    -DRTNT_BUILD_TESTS=ON
```

3. Build the library
```sh
cmake --build build/ # --parallel for faster compilation
```

4. Run the unit tests suite
```sh
ctest --test-dir build/ --output-on-failure
```

| Test name            | Status | Behavior                                                                                                                                                       |
|----------------------|:------:|----------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Handshake            |   ✅    | Verifies that a client can successfully initiate a connection and complete the handshake with a remote server.                                                 |
| Disconnect           |   ✅    | Ensures a server correctly detects when a client disconnects cleanly and triggers the associated callback.                                                     |
| Empty packet         |   ✅    | Validates transmission of packets with headers but zero payload size.                                                                                          |
| String packet        |   ✅    | Tests serialization and transmission of `std::string` payloads.                                                                                                |
| Broadcast            |   ✅    | Verifies that `server->broadcast()` successfully delivers a packet to all connected clients.                                                                   |
| Vector packet        |   ✅    | Tests serialization of `std::vector` containing primitive types.                                                                                               |
| Struct packet        |   ✅    | Tests serialization of nested [POD](https://en.wikipedia.org/wiki/Passive_data_structure) structures within a packet.                                          |
| Complex packet       |   ✅    | Tests the serializer with a heavy structure containing mixed primitives, strings, and vectors.                                                                 |
| Packet loss          |   ✅    | Simulates 65% packet loss and checks that Reliable packets eventually arrive and are complete (100% data integrity). This test works even for 90% packet loss. |
| Order                |   ✅    | Same as *Packet Loss*, but with Ordered packets. Ensures that there are no duplicates and that user receives data in order.                                    |
| Reconnection         |   ✅    | Simulates a full connection drop (100% loss) and verifies the client attempts to reconnect and restores the session once the network recovers.                 |
| Channel independency |   ✅    | Blocks Channel 1 while leaving Channel 2 open. Verifies that Channel 2 continues processing packets even while Channel 1 is stalling.                          |
| Stats                |   ✅    | Same as *Packet Loss*, but attaches a `Recorder` to export CSV metrics on bandwidth usage, RTT and retransmission rates.                                       |


## How to use

### 1. Packets

Packets are simple C++ structs. You must define a `kId` (unique identifier) and
a `kName`.    
Though `kName` is not mandatory, it is strongly recommended to define it. In all
cases, a warning will be logged if `kName` is missing.

> [!CAUTION]
> User-defined packet IDs must be in the range `128-65535`. `rtnt` reserves the
> first 127 IDs for its internal packets.  
> Giving an ID that is in the internal range (0-127) is considered a violation,
> that will ultimately raise a compiler assertion error.

Here is a simple packet implementation example:
```c++
#include "rtnt/core/packet.hpp"

struct Example
{
    static constexpr rtnt::core::packet::Id   kId   = 2204;     ///< Unique identifier of the packet.
                                                                ///  MUST be in the range 128-65535!

    static constexpr rtnt::core::packet::Name kName = "HERAS";  ///< Name that will be used in the logs.
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

Whenever a packet structure contains a custom body, it must tell `rtnt` "you
have to write THESE data on the network in THAT order".

To do that, you have to implement the `serialize` function, with all the fields
inside it:
```c++
// Inside the packet structure
template <typename Archive>
void serialize(Archive& ar)
{
    ar & field1 & field2 & field3...;
}
```

> [!CAUTION]
> Not providing a `serialize` function in a packet that contains a custom body
> will make it sterile (unsendable).  
> If you try to do so, a compiler error will be thrown.
> ```c++
> /// No body, no need to implement the serialize() function
> struct Bodiless
> {
>     static constexpr rtnt::core::packet::Id   kId   = 2204;
>     static constexpr rtnt::core::packet::Name kName = "HERAS";
> };
> 
> /// Has a body, MUST implement the serialize() function
> struct Bodied
> {
>     static constexpr rtnt::core::packet::Id   kId   = 2204;
>     static constexpr rtnt::core::packet::Name kName = "HERAS";
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
>     static constexpr rtnt::core::packet::Id kId = 2204;
>     static constexpr rtnt::core::packet::Name kName = "HERAS";
> 
>     std::string str;
> };
> ```

For now, fields can either be a primitive type (`int`, `floats`, basic 
`struct`), `std::string` or a `std::vector` (containing supported types).  
Anything else will result in undefined behavior.

### 2. Channels

`rtnt` implements a virtual channel system. This allows you to multiplex
different logic streams over the same UDP socket.

> [!CAUTION]
> You can only use channels `1-255`. `rtnt` reserves the channel 0 for its
> internal packets.  
> Using the channel 0 is considered a violation, that will ultimately raise a
> compiler assertion error.

To send a packet on a specific channel, simply specify the channel ID to use in
the packet declaration:
```c++
struct Example
{
    static constexpr rtnt::core::packet::Id        kId      = 2204;
    static constexpr rtnt::core::packet::Name      kName    = "HERAS";
    static constexpr rtnt::core::packet::ChannelId kChannel = 20;

    /// rest of packet declaration
};
```
... and everything else is done under the hood ;)

### 3. Server

Hosting a server is relatively simple. You just have to specify the port you 
want the server to be hosted on and everything else will be handled
automatically.

Whenever you're ready, call the `run()` function of both `Server` and
`asio::io_context`.  
After that, don't forget to periodically update the server with the `update()`
function to process ACKs and timeouts.

You can set a callback for each session connection/disconnection, with the 
`onConnect` and `onDisconnect` functions.

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
        logger::setThreadLabel("I/O Thread"); // Not MANDATORY but heavily recommended for easier log reading.
        ctx.run();
    });
    
    auto start = std::chrono::steady_clock::now();

    while (true) {
        server->update();   // This function can take quite some time, especially during heavy connections load.
                            // Consider implementing a dynamic sleep, will be done in further versions.
                            // You MUST call this function regularly. This handles packet reliability, ACKs and timeouts.

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / TPS));
    }

    ctx.stop();

    if (ioThread.joinable()) {
        ioThread.join();
    }
}
```

### 4. Client

Creating a client is similar to hosting a server. Simply call the `connect()` 
function with the remote server's IP and port. Everything else is done under the
hood.

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
        logger::setThreadLabel("I/O Thread"); // Not MANDATORY but heavily recommended for easier log reading.
        ctx.run();
    });
    
    auto start = std::chrono::steady_clock::now();

    while (true) {
        client->update();   // This function can take quite some time, especially during heavy connections load.
                            // Consider implementing a dynamic sleep, will be done in further versions.
                            // You MUST call this function regularly. This handles packet reliability, ACKs and timeouts.

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / TPS));
    }

    ctx.stop();

    if (ioThread.joinable()) {
        ioThread.join();
    }
}
```

### 5. Packet handling

> [!NOTE]
> As this system is still under development, it may be prone to change.

There are two ways of handling packets.

Either by having an integrated callback inside the packet structure:
```c++
struct Example
{
    static constexpr rtnt::core::packet::Id   kId   = 2204;
    static constexpr rtnt::core::packet::Name kName = "HERAS";

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
    static constexpr rtnt::core::packet::Id   kId   = 2204;
    static constexpr rtnt::core::packet::Name kName = "HERAS";
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

The `rtnt` protocol (`rtntp`) RFC can be found [here]("../../docs/rtntp.txt").

## Made with 💜 by [Lysandre B.](https://github.com/shuvlyy) ・ [![wakatime](https://wakatime.com/badge/user/2f50fe6c-0368-4bef-aa01-3a67193b63f8/project/7f2d6f99-445b-434d-a472-05df085cfd2e.svg)](https://wakatime.com/badge/user/2f50fe6c-0368-4bef-aa01-3a67193b63f8/project/7f2d6f99-445b-434d-a472-05df085cfd2e) + [![wakatime](https://wakatime.com/badge/user/2f50fe6c-0368-4bef-aa01-3a67193b63f8/project/3e3db61f-8b18-4fd0-b6be-07eff07fe80e.svg)](https://wakatime.com/badge/user/2f50fe6c-0368-4bef-aa01-3a67193b63f8/project/3e3db61f-8b18-4fd0-b6be-07eff07fe80e)
