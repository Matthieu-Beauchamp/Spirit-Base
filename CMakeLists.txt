# TODO: support installing
# https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html
# https://www.foonathan.net/2016/03/cmake-install/ 

############################################################
# cmake config
############################################################

cmake_minimum_required(VERSION 3.13)

project(Spirit-Base)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Macros)

############################################################
# C++ config
############################################################
set(CMAKE_CXX_STANDARD 20)
# Required recent features (for compiler support):
#   std::source_location


set(CMAKE_CXX_FLAGS ${SPIRIT_COMPILE_FLAGS})


############################################################
# Spirit configuration options
############################################################

# Options

spirit_define_option(
        BUILD_SHARED_LIBS
        FALSE BOOL
        "Choose if Spirit's dependencies should be linked statically
        or dynamically (On Windows, DLLs will be put with the executables automatically)"
)

spirit_define_option(
        SPIRIT_BASE_BUILD_TESTS
        FALSE BOOL
        "Select if tests should be built"
)

spirit_define_option(
        SPIRIT_BASE_BUILD_BENCHMARKS
        FALSE BOOL
        "Select if benchmarks should be built"
)

spirit_define_option(
        SPIRIT_BASE_BUILD_DOCS
        FALSE BOOL
        "Select if docs should be built"
)

spirit_define_option(
        SPIRIT_BASE_BUILD_EXAMPLES
        FALSE BOOL
        "Select if examples should be built"
)

############################################################
# Paths
############################################################
set(SPIRIT_BASE_ROOT ${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/built)
set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/lib)

set(SPIRIT_BASE_INCLUDE_DIR ${SPIRIT_BASE_ROOT}/include)
set(SPIRIT_BASE_SRC_DIR ${SPIRIT_BASE_ROOT}/src/SPIRIT)


############################################################
# Build
############################################################
spirit_update_submodules(${SPIRIT_BASE_ROOT})
add_library(spirit-base "")

# OS
# The CMake system check are more reliable than the C++ macros
#   when using Clang or Cygwin on windows for examples -> __linux__
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_compile_definitions(spirit-base PUBLIC SPIRIT_OS_WINDOWS)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_compile_definitions(spirit-base PUBLIC SPIRIT_OS_LINUX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_compile_definitions(spirit-base PUBLIC SPIRIT_OS_MACOSX)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(${MSVC})
        # Unwinding semantics disabled otherwise (Can't compile Error class)
        target_compile_options(spirit-base "/EHsc")
    endif()
endif()


# Dependencies #############################################
include(build_extLibs)

build_spdlog(${SPIRIT_BASE_ROOT}/ext_libs/spdlog spirit-base)
build_Boost(${SPIRIT_BASE_ROOT}/ext_libs/boost spirit-base)

# Build Spirit Base #############################################

# For Windows dll exports/imports (SPIRIT_API macro)
# Will this work when other modules will include / link to it?
target_compile_definitions(spirit-base PRIVATE SPIRIT_EXPORT)

target_include_directories(spirit-base PUBLIC ${SPIRIT_BASE_INCLUDE_DIR})

# TODO: Include headers as sources (helps for meta information)
set(SPIRIT_BASE_COMPONENTS 
    Logging
    Error
    Utils
)
foreach (COMPONENT ${SPIRIT_BASE_COMPONENTS})
    add_subdirectory("${SPIRIT_BASE_SRC_DIR}/${COMPONENT}")
endforeach ()


if (SPIRIT_BASE_BUILD_DOCS)
    add_subdirectory("${SPIRIT_BASE_ROOT}/docs/")
endif ()

if (SPIRIT_BASE_BUILD_EXAMPLES)
    add_subdirectory("${SPIRIT_BASE_ROOT}/examples/")
endif ()


# Expose testing facilities
if (SPIRIT_BASE_BUILD_CATCH OR SPIRIT_BASE_BUILD_TESTS)
    # May require configuring more than once because of the 
    # catch_user_config.hpp header generation
    add_subdirectory(test/Catch2)

    list(APPEND CMAKE_MODULE_PATH Catch2/extras)

    include(testing)
endif()

if (SPIRIT_BASE_BUILD_TESTS)
    add_subdirectory("${SPIRIT_BASE_ROOT}/test")
endif ()


# Expose benchmarking facilities
if (SPIRIT_BASE_BUILD_CELERO OR SPIRIT_BASE_BUILD_BENCHMARKS)
    # May require configuring more than once because of the 
    # catch_user_config.hpp header generation
    add_subdirectory(benchmarks/Celero)

    include(benchmarking)
endif()

if (SPIRIT_BASE_BUILD_BENCHMARKS)
    add_subdirectory("${SPIRIT_BASE_ROOT}/benchmarks")
endif ()