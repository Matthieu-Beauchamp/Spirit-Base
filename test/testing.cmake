
# hack to prevent CTest targets from showing up
# https://stackoverflow.com/a/57240389
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1) 
include(CTest)

include(Catch)

macro(spirit_add_test testName spiritLib ...)
    add_executable(${testName} ${...})
    target_include_directories(${testName} PRIVATE Catch2/src/)
    target_link_libraries(${testName} PRIVATE ${spiritLib} Catch2::Catch2WithMain)
    catch_discover_tests(${testName})

    set(testSrcsVar ${spiritLib}-testSrcs) # name of var
    set(${testSrcsVar} ${${testSrcsVar}} ${testName})
endmacro()

macro(spirit_test_all spiritLib)
    set(testSrcsVar ${spiritLib}-testSrcs) # name of var

    add_custom_target(${spiritLib}-test ALL
        COMMAND ctest -C $<CONFIG> --output-on-failure
        WORKING_DIRECTORY ${CMAKE_CURRENT_OUTPUT_DIR} 
        DEPENDS ${${testSrcsVar}}
    )
endmacro()
