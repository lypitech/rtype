cmake_minimum_required(VERSION 3.16)

project(rtecs
    VERSION 0.0.1
    DESCRIPTION "R-Type ECS library"
    HOMEPAGE_URL "https://github.com/lypitech/rtype"
    LANGUAGES CXX
)

# --- Options ---
option(RTECS_BUILD_TESTS "Build the test suite" OFF)

if(PROJECT_IS_TOP_LEVEL)
    message(WARNING "Building RTECS standalone, adding Shuvlog manually")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../shuvlog shuvlog)
endif()

# --- Sources / Headers ---
add_library(${PROJECT_NAME} STATIC
    src/rtecs.cpp
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
    PUBLIC shuvlog
)

# --- Output ---
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# --- Compiler settings ---
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Werror -pedantic
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.
    )
endif()

# --- Tests ---
if(RTECS_BUILD_TESTS OR PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(tests)
endif()
