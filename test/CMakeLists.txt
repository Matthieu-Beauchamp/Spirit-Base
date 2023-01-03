enable_testing()

# May require configuring more than once because of the 
# catch_user_config.hpp header generation
add_subdirectory(Catch2)

list(APPEND CMAKE_MODULE_PATH Catch2/extras)

# https://stackoverflow.com/a/57240389
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1) # hack to prevent CTest targets from showing up
include(CTest)

include(Catch)

macro(addTest testName ...)
    add_executable(${testName} ${...})
    target_include_directories(${testName} PRIVATE Catch2/src/)
    target_link_libraries(${testName} PRIVATE spirit-base Catch2::Catch2WithMain)
    catch_discover_tests(${testName})
endmacro()

addTest(AnsiEscape-test testAnsiEscape.cpp)
addTest(Concepts-test testConcepts.cpp)
addTest(fileBuf-test testFileBuf.cpp)
addTest(ansiStream-test testAnsiStream.cpp)
addTest(Logger-test testLogger.cpp)

add_custom_target(TestAll ALL
    COMMAND ctest -C $<CONFIG> --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_OUTPUT_DIR} 
    DEPENDS AnsiEscape-test Concepts-test fileBuf-test ansiStream-test Logger-test
)


