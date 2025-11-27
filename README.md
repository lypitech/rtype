<div align="center"><img src="md/assets/R-Type_Banner.png" alt="R-Type banner"></div>

# CPP-500 ‒ `R-Type`



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
Make sure to clone the repo and its submodules:
```sh
git clone --recurse-submodules https://github.com/lypitech/rtype.git
```

This project uses [`conan`](https://conan.io/) as its package manager.  
You can read how to setup `conan` in [docs/setup_conan.md](/docs/setup_conan.md).

## Build
This project uses **CMake** as its build system.

### Build & Run
```sh
# Configure and generate build files
conan install . --output-folder=build/ --build=missing -s compiler.cppstd=23
cmake -B build/

# Compile the project
cmake --build build/

# Move the resulting binary to the project root
mv build/rtype .

# Run the program
./rtype
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

### Testing
You can run tests by running:
```sh
cmake --build build/ --target test
cd build/
ctest --output-on-failure
```

## Usage instructions

## Quick-start information

## Useful links

## License
See [LICENSE](/LICENSE.md).

## Authors / contacts
louis.persin@epitech.eu  
lysandre.boursette@epitech.eu  
nathan.jeannot@epitech.eu  
pierre.marguerie@epitech.eu  
esteban.bouyault-yvanez@epitech.eu

---

###### An [<img src="https://newsroom.ionis-group.com/wp-content/uploads/2023/09/epitech-2023-logo-m.png" alt="Epitech" height=10/>](https://www.epitech.eu/) project
