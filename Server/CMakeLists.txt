cmake_minimum_required(VERSION 3.20)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

project(r-type_server
    VERSION 0.0.1
    DESCRIPTION "R-Type Server"
    HOMEPAGE_URL "https://github.com/lypitech/rtype"
    LANGUAGES CXX
)

# --- Options ---
if(PROJECT_IS_TOP_LEVEL)
    message(WARNING "Building Server standalone, adding Shuvlog and rtnt manually")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/shuvlog shuvlog)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/cli_parser cli_parser)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/rtnt rtnt)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/rteng rteng)
endif()

# --- Sources / Headers ---
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/lobby/lobby.cpp
    src/lobby/lobby_manager.cpp
    src/level_director/level_director.cpp
    src/handlers/handle_user_input.cpp
    src/app.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/cli_parser/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

find_package(asio REQUIRED)
find_package(nlohmann_json 3.12.0 REQUIRED)

# --- Libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    asio::asio
    rtnt
    rteng
    cli_parser
    shuvlog
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
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if (MSVC)
    target_compile_definitions(${PROJECT_NAME}
            PUBLIC
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            NOGDI
            NOUSER
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Werror -pedantic
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
    )
endif()
