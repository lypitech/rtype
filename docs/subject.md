## Requrements:
### CMAKE
### Package manager
- [Conan](https://conan.io)
- [Vcpkg](https://vcpkg.io)
- [CMake CPM](https://github.com/cpm-cmake/CPM.cmake)
### Cross-Platform
- Window (Microsoft Visual C++)
- no MacOS / WSL
### Documentation
- English

- README
    - Project purpose
    - Dependencies / Requirements / Supported platforms
    - Build
    - Usage instructions
    - Quick-start information
    - Useful links
    - License
    - Authors / contacts

- Developer Documentation
    - Architectural diagrams
    - Main systems overviews and description
    - Tutorials and How-To's
    - Contribution guidelines
    - Coding conventions
    - Doxygen is not enougth

- Technical and Comparative Study
    - Relevance of used technologies
    - Explain used algorithms and data structures:
    - Selection of existing algorithms
    - Design patterns
    - Data structures
    - New and custom-designed algorithms
    - Study storage techniques:
    - Persistence
    - Reliability
    - Storage constraints
    - Study Security:
    - Main vulnerabilities of each technology
    - Security monitoring in the long term

- Protocol documentation
    - RFC style (not obligatory)
    - Describe commands
    - Describe Packets
### Accessibility Requirements
- Providing a solution to:
    - Physical and motor disabilities
    - Audio and visual disabilities
    - Mental and cognitive disabilities


## <u>Delivery Part 1:</u> First Game prototype
4 weeks
- **MUST** be playable
    - Star-field in background
    - Players confront waves of enemies
    - Shoot missiles to kill the opponent
- **MUST** be a networked game
    - Multiple clients programs
    - Connect to server having final authority
- **MUST** demonstrate the foundations of a game engine
    - Visible and decoupled subsystems/layers
        - Rendering
        - Networking
        - Game Logic

### Server
Handles game logic
Has authority

- **MUST** notify each client when:
    - monster:
        - spawns
        - moves
        - is destroyed
        - fires
        - kills a player
        - ect...
    - clients:
        - moves
        - fires
        - is destroyed
        - etc...
- **MUST** be multithreaded
    - not block or wait for clients messages
    - must run frame after frame on the server regardless of clients' actions.
- **MUST** be robust
    - notify other clients that a client crashed.
    - run regardless of what's wrong

- **MAY** use the Asio for networking,
    - or rely on OS-specific network API
        - Unix BSD Sockets on Linux
        - Windows Sockets
        - (MUST be encapsulated)

### Client
graphical display of the game

- **MUST** display the game
- **MUST** handle player input
- **MAY** run parts of the game logic code
    - Local player movement
    - Missile movements
    - The server **MUST** have authority on what happens in the end.
    - Effect that have a great impact on gameplay:
        - Death of an enemy / player
        - Pickup of an item
        - etc...
    - all players have to play the same game, whose rules are driven by the server.
- **MAY** use:
    - SFML
    - SDL
    - Raylib
    - ect...
    - Libraries with a too broad scope, or existing game engines are strictly forbidden:
        - UE
        - Unity
        - Godot
        - etc...

### Protocol
- **MUST** be binary protocol
- **MUST** use UDP
- **MAY** use TCP for certain cases with justification
- malformed packets MUST NOT cause:
    - Server crash
    - Consume excessive memory
    - Compromise server security

Note: we can use [ASN.1 in BER/DER](https://en.wikipedia.org/wiki/ASN.1#Example_encoded_in_DER) for dynamic data transmition  
`type[1] length[1/1+inf] data[length]`

### Game Engine
key words:
- abstraction
- re-usable code
- decoupling (use only what's necesssary)

Create a prototype game engine

Determines:
- Representation of in-game objects
- Coordinate system
- graphics
- physics
- network

### Gameplay

- **MUST** display a slow horizontal scrolling background representing space with stars, planets...
- **MUST** use timers
    - All time-based systems **MUST NOT** be tied to the CPU speed
- **MUST** be able to move using the arrow keys
- **MUST** have enemies & missiles
- **MUST** have be monsters that spawn randomly on the right of the screen.
- The four players in a game **MUST** be distinctly identifiable
- Add sound

## <u>Delivery Part 2:</u> Advanced Topics
3 weeks
### <u>Track #1:</u> Advanced Architecture - Building a real game engine
@nathan

- use of 3rd-party libs are allowed
- can be delivered as a separate and standalone project
- create a 2nd sample game to demonstrate a reusable and generic system

#### Game Engine Features:
- Modularity
    - compile time (as flags to enable features)
    - link-time (as multiple libs)
    - run-time plugin API (load shared-object libraries)
- Engine Subsystems
    - Rendering Engine
    - Physics engine
    - Audio Engine
    - Human-Machine interface
    - Message passing interface
    - Resources & asset management
    - Scripting

#### Tooling
- Developer console
- In-game metrics and profiling
- World/scene/assets Editor
    - The quality of the editor(s) will be evaluated:  
    From a very simplistic solution using only configuration files, to a fully fledged interactive drag-and-drop graphical application.

### <u>Track #2:</u> Advanced Networking - Building a reliable network game

3 topics to choose from

#### Multi-instance Server
- Run several different game instances in parallel
- Lobby/Room system
- Users and identities management:
    - storage
    - sessions
    - authentication
    - etc...
- Communication between users:
    - text chat
    - voice chat
- Game rules management (per game instance)
- Global scoreboard
- Administration dashboard

#### Data Transmission Efficiency and Reliability
what mechanisms can you provide to help mitigate on
those network issues

- Data packing
- General-purpose data compression
    - note: useless unless for large data (json, ect)
- Network errors mitigation (packets drops, reordering, duplication)
- Message reliability
- High-level Networking Engine Architecture
    - note: interpolation
- Possible solutions
    - client-side prediction with server reconciliation
    - entity state interpolation
    - server-side lag compensation
        - input delaying
        - rollback netcode

### <u>Track #3:</u> Advanced Gameplay - Building a Fun and complete video game

make it fun to play

#### Players: elements of Gameplay
@see p21

#### Game Designers: content creation tools
@see p22
