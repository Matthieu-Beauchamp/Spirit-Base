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

# Some useful tags for my bad memory
# "-save-temps -O3 -march=native -ffast-math -Wall -Wextra"

message("-- Compile Flags: ${SPIRIT_COMPILE_FLAGS}")
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
        SPIRIT_BUILD_TESTS
        TRUE BOOL
        "Select if tests should be built"
)


############################################################
# Paths
############################################################
set(SPIRIT_BASE_ROOT ${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/built)
set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/lib)

set(SPIRIT_BASE_INCLUDE_DIR ${SPIRIT_BASE_ROOT}/include)
set(SPIRIT_BASE_SRC_DIR ${SPIRIT_BASE_ROOT}/src/SPIRIT)
set(SPIRIT_BASE_TEST_DIR ${SPIRIT_BASE_ROOT}/test)

include_directories(${SPIRIT_BASE_INCLUDE_DIR})
include_directories(${SPIRIT_BASE_ROOT}) # Access src/ dir to include .inl files

# OS
# The CMake system check are more reliable than the C++ macros
#   when using Clang or Cygwin on windows for examples -> __linux__
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_compile_definitions(SPIRIT_OS_WINDOWS)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_compile_definitions(SPIRIT_OS_LINUX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_compile_definitions(SPIRIT_OS_MACOSX)
endif()


############################################################
# Build
############################################################
spirit_update_submodules(${SPIRIT_BASE_ROOT})


if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(${MSVC})
        spirit_add_compile_flags("-DNOMINMAX") # Removes min and max macros # TODO: I dont think it caused problems in base
        spirit_add_compile_flags("/EHsc") # Unwinding semantics disabled otherwise (Can't compile Error class)
    endif()
endif()

# Dependencies #############################################
include(build_extLibs)

build_spdlog(${SPIRIT_BASE_ROOT}/ext_libs/spdlog         SPDLOG_LIB SPDLOG_INCLUDE)
build_Boost(${SPIRIT_BASE_ROOT}/ext_libs/boost           BOOST_LIB BOOST_INCLUDE BOOST_LINK)

# Build Spirit Base #############################################

add_library(spirit-base "")

# Only export when building the lib
# Will this work when other modules will include / link to it?
target_compile_definitions(spirit-base PRIVATE SPIRIT_EXPORT)

target_include_directories(spirit-base PUBLIC ${SPDLOG_INCLUDE})

target_include_directories(spirit-base PUBLIC  ${BOOST_INCLUDE})
target_link_directories(spirit-base PRIVATE ${BOOST_LINK})

target_link_libraries(spirit-base ${SPDLOG_LIB} ${BOOST_LIB})

# TODO: Include headers as sources
set(SPIRIT_BASE_COMPONENTS 
    Logging
    Utils
)
foreach (COMPONENT ${SPIRIT_BASE_COMPONENTS})
    add_subdirectory("${SPIRIT_BASE_SRC_DIR}/${COMPONENT}")
endforeach ()


if (SPIRIT_BUILD_TESTS)
    add_subdirectory(${SPIRIT_BASE_TEST_DIR})
endif ()


# Build examples
# TODO: IF (BUILD EXAMPLES)
add_subdirectory("${SPIRIT_BASE_ROOT}/examples/")

