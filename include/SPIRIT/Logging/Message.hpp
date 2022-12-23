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

#include "spdlog/fmt/ostr.h"
#include "spdlog/logger.h"

#include <experimental/source_location>

namespace sp
{

using LogLevel = spdlog::level::level_enum;

namespace details
{

template <class... Args>
using format_string_t = spdlog::format_string_t<Args...>;
using string_view_t   = spdlog::string_view_t;


// Tried to make it so we could detect if the strings passed to constructors
// are constexpr (to use format_string_t), could not make it work while still
// allowing dynamic strings. We will settle on string_view
// We could make something like Message<"myVar = {}">(myVar), but its a bit ugly
template <LogLevel lvl>
class MessageBase
{
    typedef std::experimental::source_location SourceLocation;

public:

    // Message() : Message{""} {}

    // // Final Delegate for formatting
    // template <class... Args>
    // Message(string_view_t msg, Args &&... args )
    //     : Message{fmt::vformat(msg, fmt::make_format_args(args...))}
    // {
    // }

    // // T cannot be statically converted to format string (including string_view/wstring_view)
    // template <
    //     class T,
    //     std::enable_if_t<!spdlog::is_convertible_to_any_format_string<T>::value, int> = 0>
    // Message(const T & msg)
    //     : Message{fmt::vformat("{}", fmt::make_format_args(msg))}
    // {
    // }


    // Final constructor
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


template <LogLevel lvl, class... Args>
class Message : public MessageBase<lvl>
{
    typedef std::experimental::source_location SourceLocation;
    typedef MessageBase<lvl> Base;

public:

    Message(SourceLocation loc = SourceLocation::current())
        : Base{"", loc}
    {
    }

    Message(
        string_view_t msg,
        Args &&... args,
        SourceLocation loc = SourceLocation::current()
    )
        : Base{fmt::vformat(msg, fmt::make_format_args(args...)), loc}
    {
    }

    // // T cannot be statically converted to format string (including string_view/wstring_view)
    template <
        class T,
        std::enable_if_t<!spdlog::is_convertible_to_any_format_string<T>::value, int> = 0>
    Message(const T & msg, SourceLocation loc = SourceLocation::current())
        : Base{fmt::vformat("{}", fmt::make_format_args(msg)), loc}
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
template <LogLevel lvl, class... Args>
Message(string_view_t, Args &&...) -> Message<lvl, Args...>;


} // namespace details


} // namespace sp


#endif // SPIRIT_MESSAGE_HPP
