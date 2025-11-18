cmake_minimum_required(VERSION 3.20)

project(rtype
    VERSION 0.0.0
    DESCRIPTION "R-Type Game + game engine"
    HOMEPAGE_URL "https://github.com/lypitech/rtype"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clangd

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ==============================================================================
#  Dependencies
# ==============================================================================
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)


find_package(asio REQUIRED)
find_package(raylib REQUIRED)
find_package(imgui REQUIRED)

add_subdirectory(shovologger)

# ==============================================================================
#  Source files
# ==============================================================================
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})


# ==============================================================================
#  Include paths
# ==============================================================================
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/shovologger/include
)

# ==============================================================================
#  Linking
# ==============================================================================
target_link_libraries(${PROJECT_NAME} PRIVATE
    shovologger
    asio::asio
    raylib
    imgui::imgui
)

# ==============================================================================
#  Multithreaded builds
# ==============================================================================
if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
    include(ProcessorCount)
    ProcessorCount(N)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif()

# ==============================================================================
#  Compiler flags
# ==============================================================================
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG_MODE")

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra
        -Winit-self
        -Wshadow -Wundef
        -Wredundant-decls -Wunreachable-code
        -Wwrite-strings
        -pedantic
        -Wstrict-overflow=2
        --param max-inline-insns-single=200
    )
endif()

# ==============================================================================
#  Custom build targets
# ==============================================================================
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build .
    COMMENT "Building in debug mode with DEBUG_MODE defined"
)

add_custom_target(re
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target all
)


# ==============================================================================
#  Installation
# ==============================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# ============================================================
#  Tests
# ============================================================
# enable_testing()
# 
# add_executable(test_shovologger tests/Tester3000.cpp)
# 
# target_link_libraries(test_shovologger PRIVATE shovologger)
# 
# target_include_directories(test_shovologger PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )
# 
# add_test(NAME LoggerTest COMMAND test_shovologger)
