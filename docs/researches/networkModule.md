
# R-TYPE ‒ Technical Study: Network Module

| Project | R-Type (B-CPP-500) |
| :--- | :--- |
| **Module** | Network Communication (Server & Client) |
| **Status** | Validated |

---

## 1. Context and Constraints

The core of the project relies on a distributed Client/Server architecture. The technical constraints imposed by the subject and the development environment are as follows:

1.  **Cross-Platform:** The server and client must imperatively run on Linux (rendering requirement) and Windows (bonus/cross-platform requirement).
2.  **Performance & Non-blocking:** The server must be capable of handling game logic and requests from multiple clients simultaneously without ever stopping its execution loop (Multithreading/Asynchronous).
3.  **Mixed Protocols:** Mandatory use of UDP for real-time gameplay, with tolerance for TCP for critical exchanges (connection, chat).

The subject leaves the choice between using low-level OS APIs (Native Sockets) or using the **Asio** library.

---

## 2. Comparative Analysis of Solutions

We evaluated three technical approaches to implement the network layer.

| Criteria | **"Raw" Sockets (OS API)** | **Boost.Asio** | **Asio (Standalone)** |
| :--- | :--- | :--- | :--- |
| **Type** | Native System API (Syscall). | Complete C++ Framework. | Modern C++ Library (Header-only). |
| **Portability** | **Complex**. Implementations differ between Linux (`<sys/socket.h>`) and Windows (`<winsock2.h>`). Requires heavy preprocessor directive management (`#ifdef`). | **Excellent**. The code is platform-agnostic. | **Excellent**. The code is platform-agnostic. |
| **Deployment** | **Very Light**. No external dependency. | **Heavy**. Requires installation of the full Boost suite, complicating CI/CD and compilation time. | **Light**. Requires no prior compilation (Header-only), integrates via CMake/Conan instantly. |
| **Async Model** | **Difficult**. Manual management of `select()`, `poll()`, or `epoll()`. High risk of concurrency errors. | **Native**. Uses the *Proactor* pattern via `io_context`. | **Native**. Same architecture as Boost, but decoupled. |

---

## 3. Technical Decision: Asio (Standalone)

The team validated the use of **Asio (Standalone version)**. This choice is justified by three strategic axes:

### A. Native Cross-Platform Management
Since the subject requires Linux/Windows compatibility, using raw sockets would require us to write our own abstraction layers (wrappers) to mask OS differences (WSA initialization on Windows, buffer management, etc.).
Asio provides this abstraction natively. This allows us to write a single, standardized C++ network code that compiles and executes identically on development machines (often heterogeneous) and on the Linux CI/CD.

### B. Asynchronous Architecture (IO Context)
To guarantee game fluidity, the server must never be blocking (waiting for message receipt before continuing simulation).
Asio relies on the `io_context` model. We launch asynchronous operations (e.g., `async_receive_from`) that do not block the main thread. When a packet is received, a callback function is triggered. This is the ideal architecture for a real-time game server that must maintain a constant tickrate (e.g., 60 updates/sec).

### C. Ease of Integration (vs Boost)
Unlike Boost.Asio which requires linking heavy static libraries, the Standalone version is "Header-only". It is added to the project via a simple inclusion in `CMakeLists.txt` (via Conan or Vcpkg), drastically reducing configuration and compilation times.

---

## 4. Protocol Strategy

In accordance with project directives, we will adopt a hybrid strategy:

* **UDP (User Datagram Protocol):** Used for 95% of exchanges (player positions, shots, monster spawns). Packet loss is not critical as the next one will update the state (State synchronization). This guarantees minimal latency.
* **TCP (Transmission Control Protocol):** Used only for events requiring absolute reliability (Login, Lobby, Text Chat, End game score).

---

## 5. Technical Glossary

* **Header-only:** Software library composed solely of header files (`.hpp`). It requires no separate compilation step from the user, facilitating its integration.
* **Asynchronous (Non-blocking):** Execution model where the program launches a task (e.g., network reading) and continues its work without waiting for the result. The result is processed later, when available.
* **Raw Sockets:** Direct manipulation of the operating system kernel's network interfaces. Performant but very verbose and non-portable without abstraction.
