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


#ifndef SPIRIT_ANSISTREAM_HPP
#define SPIRIT_ANSISTREAM_HPP

#include "SPIRIT/Configuration/config.hpp"
#include "AnsiEscape.hpp"
#include "SPIRIT/Concepts/Concepts.hpp"
#include "details/FileBuf.hpp"

#include <memory>
#include <sstream>

namespace sp
{


namespace details
{

class FileStream : public std::ostream
{
public:

    FileStream(FILE * file) : fileBuf{file}, std::ostream{&fileBuf} {}

    [[nodiscard]] FILE *
    file() const
    {
        return fileBuf.file();
    }


private:

    // prevent redirection
    using std::ostream::rdbuf;

    sp::details::OutFileBuf fileBuf;
};

} // namespace details


////////////////////////////////////////////////////////////
// We cannot inherit from ostream since it causes ambiguous
// overload resolutions for operator<<.
//
// Furthermore, since our AnsiSequences rely on being detected by
// a stream which knows whether or not it supports them, they always output when
// their overloaded operator<< is called with an ostream.
// This means that when the inherited operator<< is called, an ostream& is
// returned and every subsequent  << AnsiEscape is passed without regards to
// wether the stream accepts ansi escapes.
////////////////////////////////////////////////////////////


template <class StreamType>
class AnsiStreamWrapper
{
    typedef std::ostream & (*Manipulator)(std::ostream &);
    typedef StreamType InnerStream;

public:

    typedef typename StreamType::char_type char_type;
    typedef typename StreamType::int_type int_type;
    typedef typename StreamType::pos_type pos_type;
    typedef typename StreamType::off_type off_type;
    typedef typename StreamType::traits_type traits_type;

    AnsiStreamWrapper(bool enableAnsi, InnerStream && inner)
        : inner{inner}, areSequencesEnabled{enableAnsi}
    {
    }

    template <class... Args>
    AnsiStreamWrapper(bool enableAnsi, Args &&... args)
        : inner{std::forward<Args>(args)...}, areSequencesEnabled{enableAnsi}
    {
    }

    // Move are not implemented since streams are not typically movable
    // (ie ostreams does not define moves, but stringstream does)

    ~AnsiStreamWrapper() = default;


    // Allows access to stream specific behavior,
    // cannot prevent users from changing the streambuf...
    [[nodiscard]] InnerStream &
    stream()
    {
        return inner;
    }

    [[nodiscard]] const InnerStream &
    stream() const
    {
        return inner;
    }

    [[nodiscard]] InnerStream *
    operator->()
    {
        return std::addressof(inner);
    }

    [[nodiscard]] const InnerStream *
    operator->() const
    {
        return std::addressof(inner);
    }

    [[nodiscard]] bool
    isAnsiEnabled() const
    {
        return areSequencesEnabled;
    }

    void
    enableAnsi(bool on = true)
    {
        areSequencesEnabled = on;
    }

    template <class T, std::enable_if_t<sp::traits::isAnsiEscape<T>::value, bool> = true>
    friend AnsiStreamWrapper &
    operator<<(AnsiStreamWrapper & stream, T && seq)
    {
        if (stream.isAnsiEnabled())
            stream.stream() << seq;

        return stream;
    }

    template <class T, std::enable_if_t<!sp::traits::isAnsiEscape<T>::value, bool> = true>
    friend AnsiStreamWrapper &
    operator<<(AnsiStreamWrapper & stream, T && obj)
    {
        stream.stream() << obj;
        return stream;
    }

    friend AnsiStreamWrapper &
    operator<<(AnsiStreamWrapper & stream, Manipulator manip)
    {
        stream.stream() << manip;
        return stream;
    }

private:

    bool areSequencesEnabled;

    // We don't use a reference since these may go bad.
    // Also this is intended as a new stream class, not one which
    // modifies to behavior of an *existing stream object*
    // (Nuance being that we construct a new stream)
    InnerStream inner;
};


namespace traits
{
// TODO: TEST

template <class Stream, class = void>
struct AnsiWrap
{
    typedef sp::AnsiStreamWrapper<Stream> Wrapped;
};

template <class Stream>
struct AnsiWrap<Stream, sp::traits::void_t<decltype(std::declval<Stream>().stream())>>
{
    typedef Stream Wrapped;
};

template <class Stream>
using AnsiWrapped_t = typename AnsiWrap<Stream>::Wrapped;

} // namespace traits


[[nodiscard]] bool
supportsAnsi(FILE * file);

enum class ansiMode
{
    always,
    automatic,
    never
};

// TODO: Provide a way to make it streams unbuffered (sync with stdio)
// TODO: template<charType>
// TODO: Input sequences..? https://en.wikipedia.org/wiki/ANSI_escape_code#Terminal_input_sequences
class SPIRIT_API AnsiFileStream
    : public sp::AnsiStreamWrapper<sp::details::FileStream>
{
    typedef sp::details::FileStream StreamType;
    typedef sp::AnsiStreamWrapper<StreamType> Wrapper;

public:

    typedef typename Wrapper::char_type char_type;
    typedef typename Wrapper::int_type int_type;
    typedef typename Wrapper::pos_type pos_type;
    typedef typename Wrapper::off_type off_type;
    typedef typename Wrapper::traits_type traits_type;

    // Required when "double wrapping",
    // bool catches the enableAnsi parameter
    AnsiFileStream(bool, FILE * file, ansiMode mode = ansiMode::automatic)
        : AnsiFileStream{file, mode}
    {
    }

    AnsiFileStream(FILE * file, ansiMode mode = ansiMode::automatic)
        : Wrapper{false, file}
    {
        changeSequenceMode(mode);
    }


private:

    void
    changeSequenceMode(ansiMode mode)
    {
        switch (mode)
        {
            case ansiMode::always: enableAnsi(true); return;
            case ansiMode::never: enableAnsi(false); return;
            case ansiMode::automatic:
                enableAnsi(supportsAnsi(this->stream().file()));
                return;
        }
    }
};

extern SPIRIT_API sp::AnsiFileStream ansiOut;
extern SPIRIT_API sp::AnsiFileStream ansiErr;


} // namespace sp


#endif // SPIRIT_ANSISTREAM_HPP
