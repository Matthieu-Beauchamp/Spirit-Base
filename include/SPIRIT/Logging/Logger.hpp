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


#ifndef SPIRIT_LOGGER_HPP
#define SPIRIT_LOGGER_HPP

#include "SPIRIT/Configuration/config.hpp"
#include "AnsiStream.hpp"
#include "Message.hpp"
#include "spdlog/details/null_mutex.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/spdlog.h"

#include <array>
#include <memory>
#include <mutex>

namespace sp
{

////////////////////////////////////////////////////////////
// Log message levels
////////////////////////////////////////////////////////////

template <class... Args>
using Trace = details::Message<LogLevel::trace, Args...>;

template <class... Args>
using Debug = details::Message<LogLevel::debug, Args...>;

template <class... Args>
using Info = details::Message<LogLevel::info, Args...>;

template <class... Args>
using Warn = details::Message<LogLevel::warn, Args...>;

template <class... Args>
using Error = details::Message<LogLevel::err, Args...>;

template <class... Args>
using Critical = details::Message<LogLevel::critical, Args...>;


////////////////////////////////////////////////////////////
// wrapping spdlog
////////////////////////////////////////////////////////////

using Logger    = spdlog::logger;
using LoggerPtr = std::shared_ptr<sp::Logger>;


template <class Sink, class... SinkArgs>
sp::LoggerPtr
makeLogger(const std::string & name, SinkArgs &&... args)
{
    return spdlog::create<Sink>(name, std::forward<SinkArgs>(args)...);
}

sp::LoggerPtr
makeLogger(const std::string & name);


// TODO: Free functions for convenience...

// Returns the logger used by Spirit
// You a free to remove/modify its sinks, format pattern, etc.
// Note that added sinks will not share pattern, call set_pattern with
// spiritPattern() or your own:
// sp::spiritLogger()->set_pattern(sp::spiritPattern());
sp::LoggerPtr
spiritLogger();

// Get the default pattern string used by spiritLogger()
std::string
spiritPattern();

// Convenience for streaming Messages:
//
// sp::spiritLog() << sp::Info("hello");
//
inline sp::Logger &
spiritLog()
{
    return *sp::spiritLogger();
}

// set pattern reference:
// https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#customizing-format-using-set_pattern

// TODO: Naming is not great below...


////////////////////////////////////////////////////////////
/// \brief Defines how the color range for a given LogLevel is displayed
///     (see Logger, AnsiStreamSink and AnsiFileSink support color output)
////////////////////////////////////////////////////////////
struct SPIRIT_API LevelColor
    : public sp::Escapes<sp::Style, sp::FgColor, sp::BgColor> // style first otherwise might reset colors
{
private:

    typedef sp::Escapes<sp::Style, sp::FgColor, sp::BgColor> Base;

public:

    constexpr LevelColor(
        sp::FgColor fg  = sp::defaultFg,
        sp::BgColor bg  = sp::onDefault,
        sp::Style style = sp::reset // no style
    )
        : Base{style, fg, bg}
    {
    }
};

////////////////////////////////////////////////////////////
/// \brief Creates a Sink that outputs to a given ostream
///
/// The sink is meant to be used with sp::Logger,
/// the stream can be accessed using AnsiStreamWrapper methods.
///
/// The stream is made to be aware of Ansi TextStyle escapes.
/// It will not output them when ansi is disabled. (Terminal Control escape are not filtered)
///
////////////////////////////////////////////////////////////
template <class Stream, class Mutex>
class AnsiStreamSink : public spdlog::sinks::base_sink<Mutex>,
                       public sp::traits::AnsiWrapped_t<Stream>
{
    typedef spdlog::sinks::base_sink<Mutex> BaseSink;
    typedef sp::traits::AnsiWrapped_t<Stream> BaseStream;

public:

    typedef typename BaseStream::char_type char_type;
    typedef typename BaseStream::int_type int_type;
    typedef typename BaseStream::pos_type pos_type;
    typedef typename BaseStream::off_type off_type;
    typedef typename BaseStream::traits_type traits_type;

    // Args are passed to Stream constructor
    template <class... Args>
    AnsiStreamSink(
        bool enableAnsi,
        std::unique_ptr<spdlog::formatter> && formatter,
        Args &&... args
    );

    template <class... Args>
    AnsiStreamSink(bool enableAnsi, Args &&... args);

    void
    setLevelColor(LogLevel lvl, LevelColor color);

protected:

    void
    flush_() override;

    void
    sink_it_(const spdlog::details::log_msg & msg) override;

private:

    // TODO: Does not account for generic sequences (ie cursor and erase)
    // only for TextStyle sequences, AnsiSequences need further refinement
    void
    filterSequences(const char_type * start, size_t size);

    void
    write(const spdlog::memory_buf_t & formatted, size_t start, size_t end);

    // Default colors for logging levels
    static constexpr LevelColor trace{sp::white};
    static constexpr LevelColor debug{sp::cyan};
    static constexpr LevelColor info{sp::green};
    static constexpr LevelColor warn{sp::yellow, sp::onDefault, sp::bold};
    static constexpr LevelColor error{sp::red, sp::onDefault, sp::bold};
    static constexpr LevelColor critical{sp::black, sp::onRed, sp::bold};
    static constexpr LevelColor off{}; // ?

    std::array<LevelColor, spdlog::level::n_levels>
        levelColors{trace, debug, info, warn, error, critical, off};
};

template <class Stream>
using AnsiStreamSink_st = AnsiStreamSink<Stream, spdlog::details::null_mutex>;

template <class Stream>
using AnsiStreamSink_mt = AnsiStreamSink<Stream, std::mutex>;


////////////////////////////////////////////////////////////
/// \brief Creates an Ansi escapes aware sink that outputs to a FILE
///
/// Allows logging color output that will print plain text when the FILE
/// is not a terminal.
////////////////////////////////////////////////////////////
template <class Mutex>
class AnsiFileSink : public AnsiStreamSink<sp::AnsiFileStream, Mutex>
{
    typedef AnsiStreamSink<sp::AnsiFileStream, Mutex> BaseSink;

public:

    typedef typename BaseSink::char_type char_type;
    typedef typename BaseSink::int_type int_type;
    typedef typename BaseSink::pos_type pos_type;
    typedef typename BaseSink::off_type off_type;
    typedef typename BaseSink::traits_type traits_type;

    AnsiFileSink(FILE * file, ansiMode mode = ansiMode::automatic)
        : AnsiFileSink{file, std::make_unique<spdlog::pattern_formatter>(), mode}
    {
    }

    AnsiFileSink(
        FILE * file,
        std::unique_ptr<spdlog::formatter> && formatter,
        ansiMode mode = ansiMode::automatic
    )
        : BaseSink{false, std::move(formatter), file, mode}
    {
    }
};

using AnsiFileSink_st = AnsiFileSink<spdlog::details::null_mutex>;
using AnsiFileSink_mt = AnsiFileSink<std::mutex>;

} // namespace sp


#include "Logger_inl.hpp"

#endif // SPIRIT_LOGGER_HPP
