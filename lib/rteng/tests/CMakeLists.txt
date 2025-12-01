# --- Sources ---
set(RTENG_TEST_SOURCES
    Main.cpp
    tests/Initialization.cpp
)

add_executable(rteng_tests ${RTENG_TEST_SOURCES})

# --- Shuvlog build info fetch ---
target_compile_definitions(rteng_tests PRIVATE
    SHLG_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    SHLG_BUILD_VERSION="${PROJECT_VERSION}"
    SHLG_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
    SHLG_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
    SHLG_COMPILER_FLAGS="${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}"
    SHLG_BUILD_SYSTEM="CMake ${CMAKE_VERSION}"
)

# Shuvlog: For relative paths in sources.
target_compile_options(rteng_tests PRIVATE
    -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
)

# --- Headers ---
target_include_directories(rteng_tests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
)

# --- Dependencies ---
find_package(GTest REQUIRED)

target_link_libraries(rteng_tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    rteng
)

# --- GTest setup ---
include(GoogleTest)
gtest_discover_tests(rteng_tests)
