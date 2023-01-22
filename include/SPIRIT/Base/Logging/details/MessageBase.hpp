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


#ifndef SPIRIT_MESSAGEBASE_HPP
#define SPIRIT_MESSAGEBASE_HPP

#include "SPIRIT/Base/Configuration/config.hpp"
#include "SPIRIT/Base/Logging/Format.hpp"
#include "spdlog/logger.h"


#if __has_include(<source_location>)
#    include <source_location>
#elif __has_include(<experimental/source_location>)
#    include <experimental/source_location>
#else
#    error "No std::source_location is available"
#endif

namespace sp
{

using LogLevel = spdlog::level::level_enum;

namespace details
{

template <LogLevel lvl>
class MessageBase
{
public:

#if __has_include(<source_location>)
    typedef std::source_location SourceLocation;
#elif __has_include(<experimental/source_location>)
    typedef std::experimental::source_location SourceLocation;
#endif


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

    [[nodiscard]] const std::string &
    str() const
    {
        return msg;
    }

    [[nodiscard]] const SourceLocation &
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


} // namespace details

} // namespace sp


#endif // SPIRIT_MESSAGEBASE_HPP
