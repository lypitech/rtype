
# R-TYPE ‒ Technical Study: Testing Module

| Project | R-Type (B-CPP-500) |
| :--- | :--- |
| **Module** | Quality Assurance (Unit Testing) |
| **Status** | Validated |

---

## 1. Context and Requirements

The R-Type project is not just a game; it is a complex piece of software engineering that must be reliable. The project guidelines explicitly require us to adopt "good software engineering practices" and "well-defined software development workflow".

The specific constraints driving our testing strategy are:
1.  **Robustness:** The server must run continuously and handle unexpected errors or crashes from clients without stopping.
2.  **CI/CD Automation:** We must implement a Continuous Integration (CI) pipeline to automatically build and test the project after every change.
3.  **Stability:** As we are building a custom Game Engine, we need to ensure that adding a new feature (like a new monster) does not break existing features (like movement).

To meet these requirements, we have selected **Google Test (GTest)** combined with **Google Mock (GMock)**.

---

## 2. Technical Justification: Why Google Test?

We chose Google Test not just because it is popular, but because it solves specific technical challenges of the R-Type project:

### A. Simulation of Network Events (Mocking)
This is the most critical reason. Our server communicates via the network (UDP/TCP). Testing the server logic by manually launching a client every time is slow and unreliable.
**Google Mock (GMock)** is included with GTest and allows us to create "Fake" (Mock) network clients.
* *Use Case:* We can write a test that pretends to send a "Player Shoot" packet to the server. We then verify if the server created a missile entity, **without** needing a real network connection. This ensures we can validate the "Server Robustness" in isolation.

### B. Industry Standard & Employability
The subject aims to teach us "advanced development techniques". Google Test is the industry standard for C++.
* It is used by Chromium, LLVM, and thousands of companies.
* Using it proves to the evaluators that we are using professional-grade tools, not just school-level libraries.
* The documentation is extensive, which speeds up problem-solving during the short development cycle (7 weeks).

### C. Seamless CMake & CI/CD Integration
The project requires the use of **CMake** as the build system.
GTest is designed to work natively with CMake (via `FetchContent` or standard packages).
* *Benefit:* We can run all our tests with a single command (`ctest`). This makes it extremely easy to integrate into the CI/CD pipeline, ensuring that no bad code is ever merged into the main branch.

---

## 3. Technical Glossary

* **Unit Test:**
    A small, automated test that checks if a specific function works correctly.
    * *Example:* Does the function `Position::move(x, y)` actually change the coordinates of the entity?

* **Mocking (Simulation):**
    A technique to replace a complex, uncontrollable part of the system (like the Internet, a Database, or the User Input) with a controlled "Fake" object.
    * *Context:* Instead of waiting for a real player to press 'Space', the Mock says "I just pressed Space" to the server immediately.

* **Regression:**
    A bug that appears in a feature that used to work, usually caused by modifying code elsewhere.
    * *Context:* You change the code for the "Boss" monster, and suddenly the "Little" monsters stop moving. Automated tests catch this immediately.

* **CI/CD (Continuous Integration):**
    A robot (like GitHub Actions) that downloads our code, compiles it, and runs all the tests automatically every time we save our work. If a test fails, the robot alerts us.
