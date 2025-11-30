cmake_minimum_required(VERSION 3.20)

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
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../lib/rtnt rtnt)
endif()

# --- Sources / Headers ---
add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# --- Libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    shuvlog rtnt
)

# --- Output ---
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
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
