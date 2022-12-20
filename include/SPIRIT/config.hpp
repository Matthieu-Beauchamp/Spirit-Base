////////////////////////////////////////////////////////////
//
// Spirit
// Copyright (C) 2022 Matthieu Beauchamp-Boulay
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef SPIRIT_CONFIG_HPP
#define SPIRIT_CONFIG_HPP

////////////////////////////////////////////////////////////
/// \ingroup Base
/// \defgroup Configuration Configuration
///
/// \brief Spirit Library configuration options
///
/// All Macros described here should be defined by users before
/// including any headers.
/// Furthermore, only the top level (module) headers
/// in include/SPIRIT/*.hpp should be included to ensure proper
/// configuration.
///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// \brief Define the SPIRIT version
////////////////////////////////////////////////////////////
#define SPIRIT_VERSION_MAJOR 0
#define SPIRIT_VERSION_MINOR 1
#define SPIRIT_VERSION_PATCH 0

////////////////////////////////////////////////////////////
/// \brief Define DLL export/import macro
////////////////////////////////////////////////////////////
#define SPIRIT_API

// Note that these macros are defined by cmake (cmake/config.cmake),
// this is because compiler specific macros are not very reliable,
// msys/gcc would define Linux as the platform.
#if defined(SPIRIT_OS_WINDOWS)
#if defined(SPIRIT_EXPORT)
#define SPIRIT_API __declspec(dllexport)
#else
#define SPIRIT_API __declspec(dllimport)
#endif

#elif defined(SPIRIT_OS_LINUX)

#elif defined(SPIRIT_OS_MACOSX)

#else
#error "Unrecognized OS"
#endif

#define SPIRIT_TRUE 1
#define SPIRIT_FALSE 0

////////////////////////////////////////////////////////////
/// \ingroup Configuration
/// \brief Enables Error (Exception) throwing.
///
/// Setting this to SPIRIT_FALSE effectively replaces the SPIRIT_THROW
/// internal macro to simply log the error without throwing
///
/// Allows users who do not want us to throw exceptions to disable them
/// (or just for release builds)
////////////////////////////////////////////////////////////
#ifndef SPIRIT_USE_ERRORS
#define SPIRIT_USE_ERRORS SPIRIT_TRUE
#endif

////////////////////////////////////////////////////////////
/// \ingroup Configuration
/// \brief Enables stacktrace in error reports (error.what())
///
/// You may wish to disable this if boost stacktrace is having
/// a hard time linking.
///
/// Disable by defining SPIRIT_USE_STACKTRACE to SPIRIT_FALSE
////////////////////////////////////////////////////////////
#ifndef SPIRIT_USE_STACKTRACE
#define SPIRIT_USE_STACKTRACE SPIRIT_TRUE
#endif

////////////////////////////////////////////////////////////
/// \ingroup Configuration
/// \brief Enables all logging
///
/// By default everything is logged in Debug and only warnings
/// and errors will be logged in Release. (Logging is sent to stdout)
///
/// define SPIRIT_VERBOSE to SPIRIT_TRUE to log everything
////////////////////////////////////////////////////////////
#ifndef SPIRIT_VERBOSE
#define SPIRIT_VERBOSE SPIRIT_FALSE
#endif

////////////////////////////////////////////////////////////
// Define a portable debug macro
////////////////////////////////////////////////////////////
#if !defined(NDEBUG)
#define SPIRIT_DEBUG
#endif

namespace sp {

////////////////////////////////////////////////////////////
// Portable fixed-size types into Spirit
//
// This section was copied from SFML's config file
////////////////////////////////////////////////////////////

// All "common" platforms use the same size for char, short and int
// (basically there are 3 types for 3 sizes, so no other match is possible),
// we can use them without doing any kind of check

// 8 bits integer types
typedef signed char Int8;
typedef unsigned char Uint8;

// 16 bits integer types
typedef signed short Int16;
typedef unsigned short Uint16;

// 32 bits integer types
typedef signed int Int32;
typedef unsigned int Uint32;

// 64 bits integer types
#if defined(_MSC_VER)
typedef signed __int64 Int64;
typedef unsigned __int64 Uint64;
#else
typedef signed long long Int64;
typedef unsigned long long Uint64;
#endif

} // namespace sp

#endif // SPIRIT_CONFIG_HPP
