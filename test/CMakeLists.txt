# Resource Link:
#   https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html

# TODO: Use catch for testing https://github.com/catchorg/Catch2

enable_testing()

# May require configuring more than once because of the 
# catch_user_config.hpp header generation
add_subdirectory(Catch2)

list(APPEND CMAKE_MODULE_PATH Catch2/extras)

# https://stackoverflow.com/a/57240389
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1) # hack to prevent CTest targets from showing up
include(CTest)

include(Catch)

# Catch example #################################################
# # These tests can use the Catch2-provided main
# add_executable(tests test.cpp)
# target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# # These tests need their own main
# add_executable(custom-main-tests test.cpp test-main.cpp)
# target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)
################################################################


macro(addTest testName ...)
    add_executable(${testName} ${...})
    target_include_directories(${testName} PRIVATE Catch2/src/)
    target_link_libraries(${testName} PRIVATE spirit-base Catch2::Catch2WithMain)
    catch_discover_tests(${testName})
endmacro()


addTest(logging-test testFileBuf.cpp)

add_custom_target(TestAll ALL
    COMMAND ctest -C $<CONFIG> --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_OUTPUT_DIR} 
)

