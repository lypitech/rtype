# --- Sources ---
set(CLI_PARSER_TEST_SOURCES
    Main.cpp
    tests/cli_parsing.cpp
    tests/cli_types.cpp
)

add_executable(cli_parser_tests ${CLI_PARSER_TEST_SOURCES})

# --- Shuvlog build info fetch ---
target_compile_definitions(cli_parser_tests PRIVATE
    SHLG_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    SHLG_BUILD_VERSION="${PROJECT_VERSION}"
    SHLG_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
    SHLG_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
    SHLG_COMPILER_FLAGS="${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}"
    SHLG_BUILD_SYSTEM="CMake ${CMAKE_VERSION}"
)

# Shuvlog: For relative paths in sources.
target_compile_options(cli_parser_tests PRIVATE
    -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
)

# --- Headers ---
target_include_directories(cli_parser_tests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
)

# --- Dependencies ---
find_package(GTest REQUIRED)

target_link_libraries(cli_parser_tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    cli_parser
)

# --- GTest setup ---
include(GoogleTest)
gtest_discover_tests(cli_parser_tests)
