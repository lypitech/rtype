<div align="center"><img src="md/assets/R-Type_Banner.png" alt="R-Type banner"></div>

# CPP-500 ‒ `R-Type`

###### An [<img src="https://newsroom.ionis-group.com/wp-content/uploads/2023/09/epitech-2023-logo-m.png" alt="Epitech" height=10/>](https://www.epitech.eu/) project

## Project Purpose

This project is developed as part of the EPITECH Advanced C++ curriculum.  
Its primary objective is to design and implement an ECS-based game engine built on a client–server architecture.  
The engine is intended to provide a solid foundation for real-time gameplay, efficient communication between components,
and scalable system design.

To showcase and validate the engine’s features, the project includes a full recreation of the classic 1987 game
[_R-Type_](https://en.wikipedia.org/wiki/R-Type).  
This recreation serves both as a technical demonstration and as a practical benchmark, ensuring that the engine supports
entity management, networking, rendering, event handling, and other core gameplay mechanics.

## Dependencies / Requirements / Supported platforms

Make sure to clone the repository and its submodules:

```sh
git clone --recurse-submodules https://github.com/lypitech/rtype.git
```

This project uses [`Conan`](https://conan.io/) as its package manager.  
You can read how to setup `Conan` in [docs/setup_conan.md](/docs/setup_conan.md).

## Build

This project uses **CMake** as its build system.

### Build & Run

```sh
# Configure and generate build files
conan install . --output-folder=build/ --build=missing -s compiler.cppstd=23
cmake -B build/

# Compile the project
cmake --build build/
```

If you want the build to be faster (to use all of your CPU cores), simply add `--parallel` to the options!

### Other Targets

You can use the following custom build targets:

| Target  | Description                                                    |
| ------- | -------------------------------------------------------------- |
| `re`    | Rebuilds the project from scratch.                             |
| `debug` | Builds the project with debugging symbols and logging enabled. |

Usage example:

```sh
cmake --build build --target clean
cmake --build build --target debug
```

### nix

You can also run the project via [nix](https://nixos.org/download/#download-nix):
```
nix run "github:lypitech/rtype#r-type_client" --impure -- -h 127.0.0.1 -p 4242
nix run "github:lypitech/rtype#r-type_server" -- -p 4242 --config waveConfig.json
```
> [!NOTE]
> Please note the `--impure` flag as it is necessary in order to run in a non-NixOS graphical environement.

### Testing

You can run tests by running:

```sh
cmake --build build/ --target test
cd build/
ctest --output-on-failure
```

## Usage instructions

To play the game, you must launch the **Server** first, followed by one or more **Clients**.

> [!IMPORTANT]
> The command-line flags defined below are **mandatory**. Failing to provide them will cause the application to crash.

### 1. Starting the Server

The server requires a listening port to be specified using the `-p` flag.

```sh
# Syntax
./build/Server/r-type_server -p <port>

# Example: Start server on port 4242
./build/Server/r-type_server -p 4242
```

### 2. Starting the Client

The client requires both the target host IP (-h) and the target port (-p) to be specified.

```sh
# Syntax
./build/Client/r-type_client -h <ip_address> -p <port>

# Example: Connect to localhost on port 4242
./build/Client/r-type_client -h 127.0.0.1 -p 4242

# Example: Connect to a remote server
./build/Client/r-type_client -h 192.168.1.50 -p 4242
```

### Controls

Once in the game, use the following keys to pilote your ship:
| Action | Key (Keyboard) |
| :--- | :--- |
| Move | Arrow Keys |
| Exit | Escape |

## Quick-start information

Want to play immediately? Follow these steps to build and run a local game.

**1. Build the project:**
Open a terminal in the project root and run:

```sh
git clone --recurse-submodules https://github.com/lypitech/rtype.git
cd rtype
conan install . --output-folder=build/ --build=missing -s compiler.cppstd=23
cmake -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build/ --parallel
```

**2. Run the Server**
Open a **new terminal** inside the project root and run this:

```sh
./build/Server/r-type_server -p 4242
```

**3. Run the Client**
Open another **new terminal** inside the project root and run this:

```sh
./build/Client/r-type_client -h 127.0.0.1 -p 4242
```

> Note: You can open multiple terminal to run multiple clients at the same time !

## Useful links

## License

See [LICENSE](/LICENSE.md).

## Authors / contacts

louis.persin@epitech.eu  
lysandre.boursette@epitech.eu  
nathan.jeannot@epitech.eu  
pierre.marguerie@epitech.eu  
esteban.bouyault-yvanez@epitech.eu
