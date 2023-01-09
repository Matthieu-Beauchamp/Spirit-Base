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

#include <cstdint>

////////////////////////////////////////////////////////////
/// \ingroup Base
/// \defgroup Configuration Configuration
///
/// \brief Spirit Library configuration options
///
/// All Macros described here should be defined by users before
/// including any headers.
/// Furthermore, only the top level (module) headers
/// in include/SPIRIT/[module].hpp should be included to ensure proper
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
#    undef SPIRIT_API
#    if defined(SPIRIT_SHARED_LIBS)
#       if defined(SPIRIT_EXPORT)
#           define SPIRIT_API __declspec(dllexport)
#       else
#           define SPIRIT_API __declspec(dllimport)
#       endif
#    else 
#       define SPIRIT_API
#    endif

#elif defined(SPIRIT_OS_LINUX)

#elif defined(SPIRIT_OS_MACOSX)

#else
#    error "Unrecognized OS"
#endif

#define SPIRIT_TRUE  1
#define SPIRIT_FALSE 0


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
#    define SPIRIT_USE_STACKTRACE SPIRIT_TRUE
#endif

////////////////////////////////////////////////////////////
// Define a portable debug macro
////////////////////////////////////////////////////////////
#if !defined(NDEBUG)
#    define SPIRIT_DEBUG
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
#    define SPIRIT_VERBOSE SPIRIT_FALSE
#    ifdef SPIRIT_DEBUG
#        define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#    else
#        define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#    endif
#else
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif


namespace sp
{

////////////////////////////////////////////////////////////
// Portable fixed-size types into Spirit
////////////////////////////////////////////////////////////

typedef std::int8_t Int8;
typedef std::uint8_t Uint8;

typedef std::int16_t Int16;
typedef std::uint16_t Uint16;

typedef std::int32_t Int32;
typedef std::uint32_t Uint32;

typedef std::int64_t Int64;
typedef std::uint64_t Uint64;

} // namespace sp


////////////////////////////////////////////////////////////
// SPDLOG configuration
////////////////////////////////////////////////////////////

// Pretty function output for Loggers
// TODO: We should fork spdlog and modify the tweakme.hpp
#ifndef SPDLOG_FUNCTION
#    ifdef __PRETTY_FUNCTION__
#        define SPDLOG_FUNCTION __PRETTY_FUNCTION__
#    else
#        define SPDLOG_FUNCTION __FUNCTION__
#    endif
#endif


#endif // SPIRIT_CONFIG_HPP
