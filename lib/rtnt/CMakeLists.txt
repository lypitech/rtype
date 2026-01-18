cmake_minimum_required(VERSION 3.16)

option(USE_CONAN "Use Conan for dependencies" ON)

if(USE_CONAN AND EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

project(rtnt
    VERSION 1.4.1
    DESCRIPTION "R-Type Network library"
    HOMEPAGE_URL "https://github.com/lypitech/rtype"
    LANGUAGES CXX
)

# --- Options ---
option(RTNT_BUILD_TESTS "Build the test suite" OFF)

if(PROJECT_IS_TOP_LEVEL)
    message(WARNING "Building RTNT standalone, adding Shuvlog manually")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../shuvlog shuvlog)

    if(NOT DEFINED RTNT_BUILD_TESTS)
        set(RTNT_BUILD_TESTS ON)
    endif()
endif()

# --- Dependencies ---
if(USE_CONAN)
    find_package(asio REQUIRED)
    set(ASIO_TARGET asio::asio)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ASIO REQUIRED IMPORTED_TARGET asio)
    set(ASIO_TARGET PkgConfig::ASIO)
endif()

# --- Sources / Headers ---
add_library(${PROJECT_NAME} STATIC
    src/core/session.cpp
    src/core/packet.cpp
    src/core/server.cpp
    src/core/client.cpp
    src/core/peer.cpp
    src/common/utils.cpp
    src/core/dispatcher.cpp
    src/stat/recorder.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# --- Libraries ---
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${ASIO_TARGET}
    PUBLIC shuvlog
)

# --- Output ---
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# --- Compiler settings ---
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _WIN32_WINNT=0x0A00 # Windows 10
        WIN32_LEAN_AND_MEAN
    )
endif()

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra #[[-Werror]] -pedantic
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
    )
endif()

# --- Tests ---
if(RTNT_BUILD_TESTS OR PROJECT_IS_TOP_LEVEL)
    target_compile_definitions(${PROJECT_NAME} PUBLIC RTNT_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
