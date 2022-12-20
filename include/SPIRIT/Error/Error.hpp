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

#ifndef SPIRIT_ENGINE_ERROR_HPP
#define SPIRIT_ENGINE_ERROR_HPP

#include "SPIRIT/Logging/Logging.hpp"
#include "boost/stacktrace.hpp"
#include <exception>


namespace sp
{

////////////////////////////////////////////////////////////
/// \ingroup Core
/// \defgroup Errors Errors
///
/// Define Error classes used by Spirit.
/// Errors are not meant to be catched and handled.
/// They are meant to terminate the program and provide a Stacktrace
/// on program failure.
///
/// Stacktrace can be disabled with SPIRIT_USE_STACKTRACE
/// Error throwing can be disabled with SPIRIT_USE_ERRORS
/// \see Configuration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// \ingroup Errors
/// \brief Base error class of the Spirit library
///
/// Constructors take in fmt::format style arguments or a single
/// printable object (defines friend operator<<(std::ostream &, printable))
/// \see sp::format
///
/// Base error class which builds a stack trace when constructed
/// and includes it in Error::what()
///
////////////////////////////////////////////////////////////
class SPIRIT_API SpiritError : public std::exception
{
    typedef boost::stacktrace::stacktrace Stacktrace;
    typedef boost::stacktrace::frame Frame;

public:
    template <class... Args>
    SpiritError(Args &&... args) : SpiritError{}
    {
        explanation += std::string{"\nwhat(): "}
                       + sp::IO::format(std::forward<Args>(args)...);
    }


// No stack traces on windows
// TODO: I think Mingw64 has the addr2line lib,
//      so we should be able to enable stacktraces for windows too
// this would imply detecting with which lib to link boost
#if !SPIRIT_USE_STACKTRACE

    SpiritError() = default;

#else

    SpiritError()
        : explanation{std::string{"Stacktrace:\n"}
                      + stringifyStacktrace(Stacktrace{})}
    {
    }

private:
    static std::string
    stringifyStacktrace(const Stacktrace & st)
    {
        std::stringstream str;

        // print deepest at bottom
        sp::Int32 i = 0;
        for (auto it = st.rbegin(); it != st.rend(); ++it)
        {
            const auto & bt = *it;

            if (bt.name().substr(0, 16) == "sp::Error::Error")
                break;

            str << i++ << ": " << bt.name() << " at ";
            str << bt.source_file() << ":" << bt.source_line() << "\n";
        }

        return str.str();
    }

public:

#endif

    const char *
    what() const noexcept override
    {
        return explanation.c_str();
    }


    friend std::ostream &
    operator<<(std::ostream & os, const SpiritError & error)
    {
        return os << error.explanation;
    }


private:
    std::string explanation;
};


////////////////////////////////////////////////////////////
/// \ingroup Errors
/// \brief Quick error type used in SPIRIT_ASSERT(cond, formatString, args...)
///
////////////////////////////////////////////////////////////
class SPIRIT_API AssertionError : public SpiritError
{
    using SpiritError::SpiritError;
};


////////////////////////////////////////////////////////////
/// \ingroup Errors
/// \brief Logs a warning when Condition is false, other params are passed to sp::format
///
////////////////////////////////////////////////////////////
#define SPIRIT_CHECK(COND, ...)                                                \
    if (!(COND))                                                               \
    {                                                                          \
        SPIRIT_WARN(__VA_ARGS__);                                              \
    }


////////////////////////////////////////////////////////////
/// \ingroup Errors
/// \brief Handles Error throwing according to SPIRIT_USE_ERRORS
///
/// If Errors are enabled, logs and throws the error
/// If Errors are disabled, only log the error without throwing
///
/// \see Configuration
////////////////////////////////////////////////////////////
#define SPIRIT_THROW(error) SPIRIT_ERROR(error.what())

#if SPIRIT_USE_ERRORS == SPIRIT_TRUE
#    undef SPIRIT_THROW
#    define SPIRIT_THROW(error)                                                \
        SPIRIT_ERROR(error.what());                                            \
        throw(error)
#endif

////////////////////////////////////////////////////////////
/// \ingroup Errors
/// \brief Assertion macro, takes a Condition and params to sp::SpiritError constructor
///
////////////////////////////////////////////////////////////
#define SPIRIT_ASSERT(COND, ...)                                               \
    if (!(COND))                                                               \
    {                                                                          \
        sp::AssertionError err{__VA_ARGS__};                                   \
        SPIRIT_THROW(err);                                                     \
    }


} // namespace sp


#endif // SPIRIT_ENGINE_ERROR_HPP
