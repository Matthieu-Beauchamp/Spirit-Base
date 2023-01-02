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

#include "SPIRIT/config.hpp"
#include "Format.hpp"
#include "spdlog/logger.h"

#include <experimental/source_location>

namespace sp
{

using LogLevel = spdlog::level::level_enum;

namespace details
{

template <LogLevel lvl>
class MessageBase
{
    typedef std::experimental::source_location SourceLocation;

public:

    MessageBase(std::string && str, SourceLocation loc)
        : msg{std::move(str)}, loc{loc}
    {
    }

    template <LogLevel otherLevel>
    MessageBase(const MessageBase<otherLevel> & other)
        : loc{other.loc}, msg{other.msg} {};

    template <LogLevel otherLevel>
    MessageBase(MessageBase<otherLevel> && other)
        : loc{other.loc}, msg{std::move(other.msg)} {};


    template <LogLevel otherLevel>
    MessageBase &
    operator=(const MessageBase<otherLevel> & other)
    {
        loc = other.loc;
        msg = other.msg;
        return *this;
    };

    template <LogLevel otherLevel>
    MessageBase &
    operator=(MessageBase<otherLevel> && other)
    {
        std::swap(loc, other.loc);
        std::swap(msg, other.msg);
        return *this;
    };

    ~MessageBase() = default;

    const std::string &
    str() const
    {
        return msg;
    }

    const SourceLocation &
    sourceLoc() const
    {
        return loc;
    }

    friend spdlog::logger &
    operator<<(spdlog::logger & logger, const MessageBase & msg)
    {
        logger.log(
            spdlog::source_loc{
                msg.sourceLoc().file_name(),
                static_cast<int>(msg.sourceLoc().line()),
                msg.sourceLoc().function_name()},
            lvl,
            msg.str().c_str()
        );
        return logger;
    }

private:

    SourceLocation loc;
    std::string msg;
};


////////////////////////////////////////////////////////////
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
class Message : public MessageBase<lvl>
{
    typedef std::experimental::source_location SourceLocation;
    typedef MessageBase<lvl> Base;

public:

    Message(SourceLocation loc = SourceLocation::current()) : Base{"", loc} {}

    Message(
        std::string_view msg,
        Args &&... args,
        SourceLocation loc = SourceLocation::current()
    )
        : Base{sp::format(msg, std::forward<Args>(args)...), loc}
    {
    }

    // // T cannot be statically converted to format string (including string_view/wstring_view)
    template <
        class T,
        std::enable_if_t<!spdlog::is_convertible_to_any_format_string<T>::value, int> = 0>
    Message(const T & msg, SourceLocation loc = SourceLocation::current())
        : Base{sp::format("{}", msg), loc}
    {
    }
};

////////////////////////////////////////////////////////////
// Deduction Guide, see
// https://www.cppstories.com/2021/non-terminal-variadic-args/
// https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
//
// ex:
// using Source = std::experimental::source_location;
// template <typename... Ts>
// struct Log
// {
//     Log(Ts &&... ts, const Source & loc = Source::current())
//     {
//         std::cout << loc.function_name() << " line " << loc.line() << ": ";
//         ((std::cout << std::forward<Ts>(ts) << " "), ...);
//         std::cout << '\n';
//     }
// };
//
// template <typename... Ts>
// Log(Ts &&...) -> Log<Ts...>;
//
template <LogLevel lvl>
Message() -> Message<lvl>;

template <LogLevel lvl, class T>
Message(T &&) -> Message<lvl>;

template <LogLevel lvl, class... Args>
Message(std::string_view, Args &&...) -> Message<lvl, Args...>;

} // namespace details


} // namespace sp


#endif // SPIRIT_MESSAGE_HPP
