# --- Sources ---
set(RTNT_TEST_SOURCES
    main.cpp
    tests/handshake.cpp
    tests/empty_packet.cpp
    tests/string_packet.cpp
    tests/disconnect.cpp
    tests/broadcast.cpp
    tests/vector_packet.cpp
)

add_executable(rtnt_tests ${RTNT_TEST_SOURCES})

# --- Shuvlog build info fetch ---
target_compile_definitions(rtnt_tests PRIVATE
    SHLG_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    SHLG_BUILD_VERSION="${PROJECT_VERSION}"
    SHLG_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
    SHLG_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
    SHLG_COMPILER_FLAGS="${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}"
    SHLG_BUILD_SYSTEM="CMake ${CMAKE_VERSION}"
)

# Shuvlog: For relative paths in sources.
target_compile_options(rtnt_tests PRIVATE
    -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
)

# --- Headers ---
target_include_directories(rtnt_tests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
)

# --- Dependencies ---
find_package(GTest REQUIRED)
find_package(asio REQUIRED)

target_link_libraries(rtnt_tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    asio::asio
    rtnt
)

# --- GTest setup ---
include(GoogleTest)
gtest_discover_tests(rtnt_tests)
