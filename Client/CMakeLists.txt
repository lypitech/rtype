cmake_minimum_required(VERSION 3.20)

project(r-type_client
    VERSION 0.0.1
    DESCRIPTION "R-Type Client"
    HOMEPAGE_URL "https://github.com/lypitech/rtype"
    LANGUAGES CXX
)

# --- Options ---
if(PROJECT_IS_TOP_LEVEL)
    message(WARNING "Building Client standalone, adding Shuvlog and rtnt manually")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/shuvlog shuvlog)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/rtnt rtnt)
endif()

# --- Dependencies ---
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

find_package(raylib REQUIRED)
find_package(imgui REQUIRED)

# --- Sources / Headers ---
add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# --- Libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    rtnt
)

# --- Output ---
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
)

# --- Shuvlog build info fetch ---
target_compile_definitions(${PROJECT_NAME} PRIVATE
    SHLG_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    SHLG_BUILD_VERSION="${PROJECT_VERSION}"
    SHLG_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
    SHLG_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
    SHLG_COMPILER_FLAGS="${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}"
    SHLG_BUILD_SYSTEM="CMake ${CMAKE_VERSION}"
)

# --- Compiler settings ---
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Werror -pedantic
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
    )
endif()
