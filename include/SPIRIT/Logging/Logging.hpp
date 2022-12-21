////////////////////////////////////////////////////////////
//
// Spirit Engine
// Copyright (C) 2022 Matthieu Beauchamp-Boulay
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
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

#ifndef SPIRIT_ENGINE_LOGGING_HPP
#define SPIRIT_ENGINE_LOGGING_HPP

#include <iostream>

#include "SPIRIT/Concepts/Concepts.hpp"
#include "SPIRIT/config.hpp"


#if defined(SPIRIT_VERBOSE) || defined(SPIRIT_DEBUG)
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#endif

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

// must be included when defining operator<< to log with spdlog / fmt
// Templated types may require explicit formatter specialization, see below
//
// template<> // may take template args, otherwise leave empty
// struct fmt::formatter<MyType> : fmt::ostream_formatter{}
#include "spdlog/fmt/ostr.h"


namespace sp
{

////////////////////////////////////////////////////////////
/// \brief Contains our Logging / IO methods and classes
///
/// This namespace aims to remove clutter from sp:: since all of these
/// functions and constexpr entities will be included everywhere.
///
/// Also, some very common names are used. Full qualification is recommended
/// when using this namespace.
///
////////////////////////////////////////////////////////////
namespace IO
{


////////////////////////////////////////////////////////////
/// \brief Format a string according to fmt::format syntax
///
/// TODO: Link
///
/// #include "spdlog/fmt/ostr.h" // must be included to log with spdlog / fmt
///     in the file where a std::ostream & operator<< (std::ostream & os, MyClass & c)
///     function or method is defined. Otherwise, it won't be able to log
///
////////////////////////////////////////////////////////////
template <class... Args>
SPIRIT_API std::string
format(const std::string & str, Args &&... args);


template <class T, std::enable_if_t<sp::traits::Printable<T>::value, bool> = true>
SPIRIT_API std::string
format(const T & printable);


} // namespace IO


// #define SPIRIT_APPLY_LEVEL(PRINTABLE, TXT_COLOR, BACKGROUND_COLOR)             \
//     sp::IO::colorOutput(sp::IO::format(PRINTABLE), TXT_COLOR, BACKGROUND_COLOR)

// // Requires at least one parameter
// #define SPIRIT_TRACE(...)                                                      \
//     SPDLOG_TRACE(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),               \
//                                     sp::IO::defaultFg,                       \
//                                     sp::IO::onDefault))


// #define SPIRIT_INFO(...)                                                       \
//     SPDLOG_INFO(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),                \
//                                    sp::IO::green,                              \
//                                    sp::IO::onDefault))


// #define SPIRIT_WARN(...)                                                       \
//     SPDLOG_WARN(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),                \
//                                    sp::IO::yellow,                             \
//                                    sp::IO::onDefault))


// #define SPIRIT_ERROR(...)                                                      \
//     SPDLOG_ERROR(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),               \
//                                     sp::IO::red,                               \
//                                     sp::IO::onDefault))


// #define SPIRIT_CRITICAL(...)                                                   \
//     SPDLOG_CRITICAL(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),            \
//                                        sp::IO::black,                          \
//                                        sp::IO::onRed))


// namespace priv
// {
// // TODO: Users should not be able to call those, only access to spiritInit



// bool
// enableWindowsColors();


// std::shared_ptr<spdlog::logger>
// initLogging(std::ostream & os = std::cout);

// } // namespace priv


} // namespace sp


#include "Logging_inl.hpp"


#endif // SPIRIT_ENGINE_LOGGING_HPP
