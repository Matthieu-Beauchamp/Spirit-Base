# Spirit-Base
Spirit Base defines the basic features that will be shared across all of Spirit's 
modules.

While this module is not directly intended for users of Spirit,
these features are accessible.

Some generic configurations for Spirit are done throught Spirit-Base, 
notably for logging and errors.

## Key features

- Errors with stacktraces 
- Formatting (uses fmtlib)
- Ansi escapes aware streams and sinks (mostly for color output in terminals)
- Customizable logger
- Streamable log messages (no macros)

## Installation

Requires:
- Cmake 3.13
- C++20 compiler 
  - tested on GCC 12.1

Currently, cmake installation is not supported.
The recommended approach is to use Spirit-Base as a submodule to your project.

Download:
```sh
cd MyProject/libs/
git submodule add https://github.com/Matthieu-Beauchamp/Spirit-Base
git submodule update --init --recursive
```

In MyProject/CMakeLists.txt :
```cmake
add_subdirectory(libs/Spirit-Base)

add_executable(myExe main.cpp)
target_link_libraries(myExe spirit-base)
```

## Reference
To build documentation: 
```cmake
set(SPIRIT_BASE_BUILD_DOCS TRUE)
add_subdirectory(libs/Spirit-Base)
```
will reveal a Spirit-Base-docs CMake target
that will generate Doxygen html documentation in Spirit-Base/docs/Out/html/index.html
when built.

There are some examples of typical usage in the examples folder.

# Licenses
While Spirit-Base's code is licensed under the zlib license, it uses the following
modules which have differing licenses:

- spdlog: MIT license
- Boost (Boost.stacktrace): Boost Software License v1
- Catch2 (for tests only): Boost Software License v1

# Developpement
Developpement should be done throught the Spirit super-project at 
https://github.com/Matthieu-Beauchamp/Spirit since it includes
the .clang-format and allows for easy toggling of internal targets per module.

Issues should for this modules should still be posted per-module and not on the 
Spirit repository.