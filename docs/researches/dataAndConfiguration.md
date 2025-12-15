
# R-TYPE ‒ Technical Study: Data & Configuration

| Project | R-Type (B-CPP-500) |
| :--- | :--- |
| **Module** | Configuration & Data Serialization |
| **Status** | Validated |

---

## 1. Context and Requirements

A game engine is rarely "hardcoded". To avoid recompiling the entire project every time we want to change a parameter (like screen resolution, player speed, or key bindings), we need to load these values from external files.

The requirements for this configuration system are:
1.  **Human-Readable:** The files must be editable by a human (player or developer) using a simple text editor.
2.  **Structured:** The format must support complex data (lists of values, nested categories).
3.  **Ease of Integration:** The C++ library used to parse these files must be lightweight and easy to integrate into our build system (CMake).

To meet these needs, we chose the **JSON** format and the **nlohmann/json** library.

---

## 2. Format Selection: Why JSON?

We compared several standard configuration formats:

* **INI:** Too simple. It does not natively support lists or nested structures (hierarchy), which limits us for complex engine settings.
* **XML:** Too verbose. The syntax is heavy (`<value>10</value>`), making files large and harder to read/edit manually.
* **YAML:** Readable, but sensitive to indentation. Parsing libraries for C++ are often heavy or complex to set up.
* **JSON:** **Selected.** It offers the perfect balance. It is hierarchically structured, widely used in the industry (Web, APIs, Games), and strictly standardized.

---

## 3. Technical Decision: nlohmann/json

For parsing JSON in C++, the standard choice is the library commonly known as **"JSON for Modern C++"** (by Niels Lohmann).

### A. "Modern C++" Syntax
The main strength of this library is that it treats JSON objects like standard C++ containers (`std::map`, `std::vector`).
* *Ease of Use:* We can write code like `config["resolution"]["width"] = 1920;`.
* *Readability:* The code remains clean and intuitive, drastically reducing the learning curve for the team compared to older libraries (like RapidJSON) that require complex buffer allocations.

### B. Integration (Header-Only)
Just like Asio (Standalone), `nlohmann/json` is a **Header-Only** library.
* It consists of a single file (`json.hpp`).
* It requires no complex linking or compilation steps.
* It integrates instantly with **CMake** and package managers (Conan/Vcpkg), complying with the project's requirement to use a package manager for dependencies.

### C. Type Safety
C++ is a strongly typed language, while JSON is not. This library provides safe methods to convert data.
* *Safety:* If we try to load text into an integer variable, the library will throw a clear error instead of crashing silently or corrupting memory. This improves the overall stability of the engine.

---

## 4. Use-cases in R-Type

We will use this library for two critical aspects:

1.  **Engine Configuration (`config.json`):**
    * Display settings (Window vs Fullscreen, Resolution).
    * Audio settings (Music/SFX Volume).
    * Input mapping (defining which key does what).

2.  **Game Data (Future Extensibility):**
    * Although not immediate, this library allows us to define game levels or monster waves in JSON files. This means Game Designers could modify the game rules without touching the C++ code, a key feature of a real Game Engine.

---

## 5. Technical Glossary

* **Serialization:**
    The process of converting a C++ object (like a `PlayerConfig` struct in memory) into a text format (a JSON string) that can be saved to a file.

* **Deserialization:**
    The reverse process: reading a text file and converting it back into C++ variables that the program can use.

* **Header-Only:**
    A library that does not need to be compiled into a `.lib` or `.a` file. You simply include the source code in your project, and it works.

* **Syntax Sugar:**
    A programming feature that makes code easier to read and write. `nlohmann/json` provides "syntax sugar" by allowing us to use brackets `["key"]` instead of calling complex functions like `getObject()->getField("key")->getValue()`.
