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


#ifndef SPIRIT_LOGGER_INL_HPP
#define SPIRIT_LOGGER_INL_HPP

#include "Logger.hpp"
#include "SPIRIT/Base/Error/Error.hpp"
#include "spdlog/pattern_formatter.h"

namespace sp
{

template <class Stream, class Mutex>
template <class... StreamArgs>
AnsiStreamSink<Stream, Mutex>::AnsiStreamSink(
    bool enableAnsi,
    std::unique_ptr<spdlog::formatter>&& formatter,
    StreamArgs &&... args
)
    : BaseSink{std::move(formatter)}, BaseStream{enableAnsi, std::forward<StreamArgs>(args)...}
{
}

template <class Stream, class Mutex>
template <class... StreamArgs>
AnsiStreamSink<Stream, Mutex>::AnsiStreamSink(bool enableAnsi, StreamArgs &&... args)
    : BaseSink{std::make_unique<spdlog::pattern_formatter>()},
      BaseStream{enableAnsi, std::forward<StreamArgs>(args)...}
{
}

template <class Stream, class Mutex>
void
AnsiStreamSink<Stream, Mutex>::setLevelColor(LogLevel lvl, LevelColor color)
{
    this->levelColors[lvl] = color;
}

template <class Stream, class Mutex>
void
AnsiStreamSink<Stream, Mutex>::flush_()
{
    this->stream().flush();
}

template <class Stream, class Mutex>
void
AnsiStreamSink<Stream, Mutex>::sink_it_(const spdlog::details::log_msg & msg)
{
    // mutex is locked during call to log() which calls here
    // std::lock_guard<mutex_t> lock(mutex_);

    msg.color_range_start = 0;
    msg.color_range_end   = 0;
    spdlog::memory_buf_t formatted;
    this->formatter_->format(msg, formatted);

    if (this->isAnsiEnabled() && msg.color_range_end > msg.color_range_start)
    {
        // before color range
        write(formatted, 0, msg.color_range_start);
        // in color range
        *this << levelColors[msg.level];
        write(formatted, msg.color_range_start, msg.color_range_end);
        *this << sp::reset;
        // after color range
        write(formatted, msg.color_range_end, formatted.size());
    }
    else // no color
    {
        write(formatted, 0, formatted.size());
    }
}


// TODO: Does not account for generic sequences (ie cursor and erase)
// only for TextStyle sequences, AnsiSequences need further refinement
template <class Stream, class Mutex>
void
AnsiStreamSink<Stream, Mutex>::filterSequences(const char_type * start, size_t size)
{
    constexpr char_type seqBegin = static_cast<char_type>(TextStyle::ESC);
    constexpr char_type seqEnd = static_cast<char_type>(TextStyle::end);

    const char_type * const last = start + size;

    // start and end of the range to write (not of ansi sequences)
    const char_type * beg = start;
    const char_type * end = std::find(beg, last, seqBegin);
    this->stream().write(beg, end - beg);

    while (end != last)
    {
        beg = std::find(end, last, seqEnd);
        beg++;

        // start from end in case a sequence was missing a seqEnd
        end = std::find(end + 1, last, seqBegin);

        if (beg == last || beg > end)
            throw sp::SpiritError{
                "Missing an AnsiEscape termination or "
                "unsupported sequence used in logging: {}",
                start};

        this->stream().write(beg, end - beg);
    }
}

template <class Stream, class Mutex>
void
AnsiStreamSink<Stream, Mutex>::write(
    const spdlog::memory_buf_t & formatted,
    size_t start,
    size_t end
)
{
    if (this->isAnsiEnabled())
        this->stream().write(formatted.data() + start, end - start);
    else
        this->filterSequences(formatted.data() + start, end - start);
}


} // namespace sp


#endif // SPIRIT_LOGGER_INL_HPP
