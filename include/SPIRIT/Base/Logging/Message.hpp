////////////////////////////////////////////////////////////
//
// Spirit
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


#ifndef SPIRIT_MESSAGE_HPP
#define SPIRIT_MESSAGE_HPP

#include "details/MessageBase.hpp"

namespace sp
{

////////////////////////////////////////////////////////////
/// \ingroup Logging
/// \defgroup Messages Messages
/// \brief Loggable messages to be streamed to a Logger
/// 
/// Messages contains a user specified message and source location
/// 
/// They are streamed to a logger:
/// \code sp::Logger{...} << sp::Message<lvl, ...>{...}; \endcode
/// 
/// The std::source_location allows to use Messages for logging
/// instead of macros.
/// 
////////////////////////////////////////////////////////////

using LogLevel = spdlog::level::level_enum;

////////////////////////////////////////////////////////////
/// \ingroup Messages
/// \brief Allows constructing loggable message with source location
///
/// The location where the message is created will be kept,
/// this includes the file, line and enclosing function name.
///
/// The arguments for construction can be anything from a string,
/// an object with an operator<< overload with ostream to
/// a format string with multiple format arguments:
/// \code sp::Info{"{} != {}", 1, 2}; \endcode
///
/// See sp::format
///
/// The intended usage is to stream messages to a logger:
/// \code
/// sp::Logger << sp::Info{"Some meaningful information: {}", someClass};
///
/// // Ansi escape sequences are supported (see the Sinks):
/// sp::Logger << sp::Error{"Operation failed: {}{} != {}{}",
///                         sp::red, 1, 2, sp::reset};
/// \endcode
////////////////////////////////////////////////////////////
template <LogLevel lvl, class... Args>
class Message : public sp::details::MessageBase<lvl>
{
    typedef sp::details::MessageBase<lvl> Base;
    typedef typename Base::SourceLocation SourceLocation;

public:
    /// Anything that can be passed to sp::format, including nothing:
    /// \code sp::Message{} \endcode
    /// \code sp::Message{MyPrintableType{}} \endcode
    /// \code sp::Message{"hello!"} \endcode
    /// \code sp::Message{"hello {}!", yourName} \endcode
    Message(
        Args &&... args,
        SourceLocation loc = SourceLocation::current()
    )
        : Base{sp::format(std::forward<Args>(args)...), loc}
    {
    }

};

template <LogLevel lvl, class... Args>
Message(Args &&...) -> Message<lvl, Args...>;


////////////////////////////////////////////////////////////
// Log message levels
////////////////////////////////////////////////////////////

/// \ingroup Messages
/// \{

template <class... Args>
using Trace = sp::Message<LogLevel::trace, Args...>;

template <class... Args>
using Debug = sp::Message<LogLevel::debug, Args...>;

template <class... Args>
using Info = sp::Message<LogLevel::info, Args...>;

template <class... Args>
using Warn = sp::Message<LogLevel::warn, Args...>;

template <class... Args>
using Error = sp::Message<LogLevel::err, Args...>;

template <class... Args>
using Critical = sp::Message<LogLevel::critical, Args...>;

/// \}

} // namespace sp


#endif // SPIRIT_MESSAGE_HPP
