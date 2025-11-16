# CPP-500 ‒ `R-Type`

## Requrements:
- **CMAKE**
- **Package manager**
    - [Conan](conan.io)
    - [Vcpkg](vcpkg.io)
    - [CMake CPM](https://github.com/cpm-cmake/CPM.cmake)
- **Cross-Platform**
    - Window (Microsoft Visual C++)
    - no MacOS / WSL
- **Documentation**
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
- **Accessibility Requirements**
    - Providing a solution to:
        - Physical and motor disabilities
        - Audio and visual disabilities
        - Mental and cognitive disabilities


## Delivery
### <u>Part 1:</u> First Game prototype
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

#### Server
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

#### Client
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

#### Protocol
- **MUST** be binary protocol
- **MUST** use UDP
- **MAY** use TCP for certain cases with justification
- malformed packets MUST NOT cause:
    - Server crash
    - Consume excessive memory
    - Compromise server security

Note: we can use [ASN.1 in BER/DER](https://en.wikipedia.org/wiki/ASN.1#Example_encoded_in_DER) for dynamic data transmition  
`type[1] length[1/1+inf] data[length]`

#### Game Engine
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

#### Gameplay

- **MUST** display a slow horizontal scrolling background representing space with stars, planets...
- **MUST** use timers
    - All time-based systems **MUST NOT** be tied to the CPU speed
- **MUST** be able to move using the arrow keys
- **MUST** have enemies & missiles
- **MUST** have be monsters that spawn randomly on the right of the screen.
- The four players in a game **MUST** be distinctly identifiable
- Add sound

### <u>Part 2:</u> Advanced Topics
3 weeks

---

###### An <img src="https://newsroom.ionis-group.com/wp-content/uploads/2023/09/epitech-2023-logo-m.png" alt="Epitech" height=10/> project
