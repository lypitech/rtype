cmake_minimum_required(VERSION 3.20)

project(r-type
    VERSION 0.0.1
    DESCRIPTION "CPP-500 ‒ R-Type"
    HOMEPAGE_URL "https://github.com/lypitech/rtype"
    LANGUAGES CXX
)

option(BUILD_TEST "Build the unit tests" OFF)

# --- Dependencies ---
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
        include("${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    else()
        message(FATAL_ERROR "Conan toolchain not found.")
    endif()
endif()

# --- Build options ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


find_package(asio REQUIRED)
find_package(raylib REQUIRED)
find_package(imgui REQUIRED)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    include(GoogleTest)
    message(STATUS "Building with Tests ENABLED")
endif()

add_subdirectory(lib/shuvlog)
add_subdirectory(lib/rtnt)
add_subdirectory(lib/cli_parser)
add_subdirectory(lib/rtecs)
add_subdirectory(lib/rteng)

add_subdirectory(Server)
add_subdirectory(Client)
#add_subdirectory(Editor)

# --- Build targets ---
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build .
    COMMENT "Building in debug mode with DEBUG_MODE defined"
)

add_custom_target(re
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target all
)

# --- Installation ---
#install(TARGETS ${PROJECT_NAME}
#    RUNTIME DESTINATION bin
#)
