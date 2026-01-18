<div align="center">
  <img src="md/assets/R-Type_Logo_2048.png" alt="R-Type Logo" height="120">

# CPP-500 ‒ `R-Type`

**A multithreaded networked ECS-based Game Engine & R-Type recreation.**

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-23-blue.svg?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/23)
[![License](https://img.shields.io/badge/license-Zlib-green.svg?style=flat-square)](LICENSE.md)
[![Build Status](https://img.shields.io/github/actions/workflow/status/lypitech/rtype/ci-cd.yml?branch=main&style=flat-square)](https://github.com/lypitech/rtype/actions)

  <p>
    <a href="#about">About</a> •
    <a href="#compatibility">Compatibility</a> •
    <a href="#getting-started">Getting Started</a> •
    <a href="#documentation">Documentation</a> •
    <a href="#team">Team</a>
  </p>
</div>

---

## About

This project is developed as part of the [Epitech](https://www.epitech.eu/) 
curriculum, Advanced C++ unit (Year 3).  

The primary goal of it is to design and implement a robust game engine 
featuring:
- **An ECS ([`rtecs`](lib/rtecs))**: A high-performance Entity Component System 
  for modular game logic.
- **High level network library ([`rtnt`](lib/rtnt))**: A cross-platform, 
  asynchronous network library built on [Asio](https://think-async.com/Asio/) 
  providing reliable UDP communication.
- **Multithreading**: Decoupled game logic, rendering, and network I/O.
- **Cross-platform support**: Can run on macOS, Linux and Windows on `arm64` and 
  `x86_64` CPU architectures.

To demonstrate the engine, we have recreated the mechanics of the classic 1987 
arcade game [R-Type](https://en.wikipedia.org/wiki/R-Type), serving as a 
benchmark for real-time multiplayer performance.

## Compatibility

This project is developed using **C++23**.  
It has been strictly tested and validated on the following environments:

| Platform                                       | Compiler / Toolchain         | CMake          | Status |
|:-----------------------------------------------|:-----------------------------|:---------------|:------:|
| macOS (`arm64`) 26.2 Tahoe                     | AppleClang `17.0.0.17000603` | `4.1.2`        |   ✅    |
| Linux (`x86_64`) Xubuntu 25.10                 | GNU `15.2.0`                 | `3.31.6`       |   ✅    |
| Windows (`x86_64`) 10 IoT Enterprise LTSC 2024 | MSVC `19.50.35718.0`         | `4.11.1-msvc1` |   ✅    |

> [!WARNING]
> While other configurations might work, they are not officially supported.
> Ensure your environment matches the C++23 requirements.

## Getting started

### Prerequisites

- [Git](https://git-scm.com)
- C++ compiler supporting C++23 ([GCC](https://gcc.gnu.org) 10+,
    [Clang](https://clang.llvm.org) 10+,
    [MSVC](https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B) 19.28+)
- [CMake](https://cmake.org) 3.20+
- [Conan](https://conan.io/) 2.0+

### Installation & Build

> [!NOTE]
> Make sure to properly [setup Conan](docs/setup_conan.md) on your machine 
> before.

1. Clone the repository along with its submodules:
    ```sh
    git clone --recurse-submodules https://github.com/lypitech/rtype.git
    cd rtype
    ```

2. Install dependencies:
    ```sh
    conan install . --output-folder=build/ --build=missing -s compiler.cppstd=23
    ```

3. Compile the project using CMake:
    ```shell
    # Generate build files
    cmake -B build/ -DCMAKE_BUILD_TYPE=Release
    
    # Build (use --parallel for faster compilation)
    cmake --build build/ --parallel
    ```

Server and client binaries will respectively be stored in 
`./build/Server/r-type_server` and `./build/Client/r-type_client`.

### Using Nix

If you are a Nix user, you can run the project directly:
```shell
nix run "github:lypitech/rtype#r-type_server" -- ...
nix run "github:lypitech/rtype#r-type_client" --impure -- ...
```

> [!NOTE]
> Please note the `--impure` flag as it is necessary in order to run in a 
> non-NixOS graphical environement.

### Usage

This project follows a Client-Server architecture.  
You MUST start the Server before any Clients.

1. Start the server
    ```shell
    # Usage: ./r-type_server -p <PORT> --config <CONFIG PATH>
    ./build/Server/r-type_server -p 4242 --config waveConfig.json
    ```

2. Start a client
    ```shell
    # Usage: ./r-type_client -h <SERVER_IP> -p <SERVER_PORT>
    ./build/Client/r-type_client -h 127.0.0.1 -p 4242
    ```

### Controls

| Action | Input                                               |
|--------|-----------------------------------------------------|
| Move   | <kbd>↑</kbd> <kbd>↓</kbd> <kbd>←</kbd> <kbd>→</kbd> |
| Shoot  | <kbd>Space</kbd>                                    |
| Exit   | <kbd>Esc</kbd>                                      |

## Testing
You can run the unit tests suite by running:

```sh
cmake --build build/ --target test
cd build/

ctest --output-on-failure
```

## Documentation

For deeper technical details regarding the engine's modules and research, please 
refer to our internal documentation or the Wiki.

- [Research papers](docs/researches)
- Network library (`rtnt`):
  - [Library README](lib/rtnt/README.md)
  - [Protocol RFC (`rtntp`)](docs/rtntp.txt)
- Entity Component System (`rtecs`):
  - [Library README](lib/rtecs/README.md)
- General:
  - [Subject PDF](docs/B-CPP-500_rtype.pdf)
  - [Appendix PDF](docs/B-CPP-500_rtype_apendix.pdf)

## License

This project is licensed under the zlib/libpng License. 
See the [LICENSE](/LICENSE.md) file for details.

## Team

|                             <img src="https://avatars.githubusercontent.com/u/146085057?v=4" width=92>                             |                            <img src="https://avatars.githubusercontent.com/u/123988037?v=4" width=92>                            |                     <img src="https://avatars.githubusercontent.com/u/83085376?v=4" width=92>                     |                     <img src="https://avatars.githubusercontent.com/u/122123024?v=4" width=92>                      |                                   <img src="https://avatars.githubusercontent.com/u/146708962?v=4" width=92>                                    |
|:----------------------------------------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------------------------------------------:|
| [**Pierre MARGUERIE**](https://github.com/PierreMarguerie)<br/>[*pierre.marguerie@epitech.eu*](mailto:pierre.marguerie@epitech.eu) | [**Lysandre BOURSETTE**](https://github.com/Shuvlyy)<br/>[*lysandre.boursette@epitech.eu*](mailto:lysandre.boursette@epitech.eu) | [**Nathan JEANNOT**](https://github.com/nl1x)<br/>[*nathan.jeannot@epitech.eu*](mailto:nathan.jeannot@epitech.eu) | [**Louis PERSIN**](https://github.com/electroniciv)<br/>[*louis.persin@epitech.eu*](mailto:louis.persin@epitech.eu) | [**Esteban BOUYAULT-YVANEZ**](https://github.com/Babouye)<br/>[*esteban.bouyault-yvanez@epitech.eu*](mailto:esteban.bouyault-yvanez@epitech.eu) |

<div align="right">
An <a href="https://www.epitech.eu/"><img src="https://newsroom.ionis-group.com/wp-content/uploads/2023/09/epitech-2023-logo-m.png" alt="Epitech" height=14/></a> project
</div>
