
# R-TYPE ‒ Technical Study: Developer Tools (UI)

| Project | R-Type (B-CPP-500) |
| :--- | :--- |
| **Module** | Debugging Tools & Overlay Interface |
| **Tool Selected** | Dear ImGui |
| **Status** | Validated |

---

## 1. Context and Strategic Shift

Initially, the project ambition included creating a full WYSIWYG (« What You See Is What You Get ») editor similar to Godot or Unity. However, to ensure the success of the project, we saw the **Network system** features (prediction, interpolation, rollback) was already way more developed, the team has decided to refocus resources on network stability rather than content creation tools.

Despite this shift, a Graphical User Interface (GUI) remains mandatory.
**Why?** In a real-time multiplayer environment, standard debugging methods (breakpoints, logging to console) are inefficient or impossible to use:
* **Breakpoints:** Pausing the code causes immediate network timeouts and disconnection from the server.
* **Console Logs:** The high frequency of network packets (60Hz) floods the standard output, making text logs unreadable.

Therefore, we require a **lightweight, immediate-mode GUI** to visualize the internal state of the engine in real-time.

---

## 2. Technical Justification: Dear ImGui

We have selected **Dear ImGui** (Immediate Mode GUI) to act as our runtime debugger. Here is the technical justification for its inclusion:

### A. Non-Intrusive Network Debugging
This is the primary reason for using ImGui. It allows us to build a **"Network Dashboard"** overlay directly on top of the game.
* *Real-time Visualization:* We can display graphs of Round-Trip Time (RTT), Packet Loss %, and Jitter in real-time.
* *Simulation:* We can add sliders to artificially induce lag or packet drop on the client side to test how the interpolation algorithm reacts, *while the game is running*.
* This fulfills the project requirement for **"In-game metrics and profiling"** and **"Developer Console"**.

### B. ECS Inspection (Data-Driven Debugging)
Our engine uses an ECS (Entity Component System) architecture. Debugging an ECS is notoriously difficult because data is scattered across components.
* ImGui allows us to create an **"Entity Inspector"** window.
* We can click on an entity (e.g., a Player or a Missile) and see all its attached components and values (Position, Velocity, Health) dynamically updating.
* This transforms debugging from "guessing via logs" to "seeing the state."

### C. Architecture Compatibility (Immediate Mode)
Since we chose **Raylib** (which is an Immediate Mode graphics library), **Dear ImGui** (which is an Immediate Mode UI library) fits perfectly into the rendering loop.
* It requires no object state retention.
* It sits on top of the existing render pass.
* It is extremely lightweight and has near-zero performance cost when windows are collapsed.

---

## 3. Implementation Strategy

We will use **rlImGui**, a backend binding that connects Dear ImGui specifically to Raylib.
The interface will be strictly reserved for **Developer Tools** (Debug builds) and will provide:
1.  **Console:** A command line to execute server commands directly.
2.  **Profiler:** A visual display of FPS and memory usage.
3.  **Network Overlay:** Real-time stats on connection quality.

---

## 4. Technical Glossary

* **Immediate Mode GUI:**
    A UI paradigm where the interface is redrawn from scratch every single frame. It is stateless (you don't create a "Button Object", you just call `if (Button("Click Me"))`). This makes it perfect for game engines where the state changes constantly.

* **RTT (Round-Trip time):**
This is the exact time (in milliseconds) it takes for a message to leave your Client, arrive at the Server, be processed, and return to the Client.

* **Jitter:**
That's the variance (instability) of your Ping :
	* *Good connection:* Your ping is stable at 50ms (50, 51, 50, 50, 49). Jitter is close to 0.
	* *Bad connection:* Your ping fluctuates wildly (20ms, then 150ms, then 30ms, then 200ms). Jitter is high.

* **Runtime Inspection:**
    The ability to look at the value of variables (health, speed, position) while the software is actually running, without pausing it.

* **Overlay:**
    A layer of graphics drawn *on top* of the game world. ImGui draws its windows over the Raylib game scene, allowing the developer to see both the game and the debug info simultaneously.

* **Heisenbug:**
    A software bug that seems to disappear or alter its behavior when you try to study it (e.g., when you use a breakpoint). Networking bugs are often Heisenbugs; ImGui helps catch them by being non-intrusive.
