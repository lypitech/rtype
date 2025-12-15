
# R-TYPE ‒ Technical Study: Graphics Module

| Project | R-Type (B-CPP-500) |
| :--- | :--- |
| **Module** | Graphics Client |
| **Status** | Validated |

---

## 1. Context and Constraints

The subject requires the development of a networked game separated into two distinct parts:
1.  **The Server**: Central authority, manages logic and collisions.
2.  **The Client**: Graphical display and input management (keyboard/mouse).

The client must not contain any critical game logic (such as monster death or damage); it must merely display what the server sends it. The subject explicitly authorizes the use of libraries such as **SFML**, **SDL**, or **Raylib**.

---

## 2. Comparative Analysis of Solutions

We compared three candidate libraries for the client implementation.

| Criteria | **SFML** (Simple and Fast Multimedia Library) | **SDL2** (Simple DirectMedia Layer) | **Raylib** |
| :--- | :--- | :--- | :--- |
| **Language & Style** | **C++ (Object-Oriented)**. Uses native classes and methods. | **C (Low Level)**. Manual memory and structure management. | **C (C++ Wrap)**. "Immediate Mode" style (see glossary). |
| **Productivity** | **Average**. Clean API but imposes a sometimes rigid code structure (state management). | **Low**. Requires a lot of configuration code ("boilerplate") to display a simple window. | **High**. Very few lines of code needed for an immediate visual result. |
| **macOS Compatibility** | **Complex**. Recent issues with framework management, dynamic paths (`@rpath`), and Apple Silicon architecture. | **Average**. Stable installation but requires sometimes heavy project configuration. | **Excellent**. Compiles easily statically, avoiding external dependency issues. |
| **Architecture** | Tendency to couple logic and image (e.g., the `Ship` object stores its `Sprite`). | Very flexible, but leaves all architectural responsibility to the developer. | **Decoupled**. Encourages strict separation between data (Server) and drawing (Client). |

---

## 3. Technical Decision: Raylib

The team decided to use **Raylib** for the graphics client development. This choice meets the subject constraints and the team's needs:

### A. Decoupled Architecture (Subject Requirement)
The subject requires the game engine to demonstrate a decoupled architecture (Logic/Render separation).
Raylib operates in **"Immediate Mode"**. Instead of creating and storing complex graphical objects (like SFML Sprites) that persist in memory, the client receives data from the server (e.g., "Position X=10, Y=20") and draws the corresponding texture instantly at each frame. This ensures the client remains a simple "display" without logic, in accordance with requirements.

### B. Development Environment (macOS)
Although the project must run on Linux, part of the team develops on macOS. Raylib offers the best development experience on this OS:
* No complex external framework management.
* Simplified native static compilation.
* Native support for Apple Silicon architectures (M1/M2/M3).

This ensures that every team member can compile and test the project without wasting time on environment configuration.

### C. Prototyping Speed
The deadline for the first delivery (functional prototype) is short (4 weeks). The simplicity of the Raylib API allows displaying sprites and handling keyboard inputs in a few lines, freeing up time to focus on complex tasks: UDP networking and multithreading.

---

## 4. Technical Glossary

* **Boilerplate (Boilerplate code):**
    Refers to repetitive and mandatory lines of code that do not produce direct value but are necessary to make a tool work (e.g., 50 lines to initialize a window in SDL2 vs 1 line in Raylib).

* **Immediate Mode:**
    Graphics rendering paradigm.
    * *Retained Mode (e.g., SFML):* You create an object, configure it, and the library keeps it in memory for display until you destroy it.
    * *Immediate Mode (e.g., Raylib):* You do not keep graphic state. At each game loop, you reissue the draw command: "Display the player image here". It is ideally suited for fast network synchronization.

* **Static Compilation:**
    Method where the library code (Raylib) is included directly inside the game executable (`r-type_client`).
    * *Advantage:* The game works everywhere without needing to install libraries on the user's or grader's computer.
