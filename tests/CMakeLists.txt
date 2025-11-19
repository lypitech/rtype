find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

# ==============================================================================
#  Create rtype library
# ==============================================================================
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    # ...
)
# Avoid multiple main definitions
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

if(TEST_SOURCES)
    add_library(rtype_lib ${TEST_SOURCES})
    target_include_directories(rtype_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        # ...
    )
    target_link_libraries(rtype_lib PUBLIC asio::asio raylib imgui::imgui)
endif()

# ==============================================================================
# Collect all test files
# ==============================================================================
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS "*.cpp")

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})

    target_link_libraries(${TEST_NAME}
        GTest::gtest
        GTest::gtest_main
    )

    if(TARGET rtype_lib)
        target_link_libraries(${TEST_NAME} rtype_lib)
    endif()

    gtest_discover_tests(${TEST_NAME})
endforeach()
