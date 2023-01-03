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
/// \ingroup Logging
/// \brief Wraps any ostream into an ansi aware stream
///
/// Wrapping streams should be done with AnsiWrapped_t<StreamType> to avoid
/// wrapping a previously wrapped stream.
///
/// An inner stream object will be constructed. Does not use references
/// to existing streams.
///
/// The inner stream can be accessed with .stream() or operator->.
///
/// Filters AnsiEscapes by using operator<< overloads.
///
////////////////////////////////////////////////////////////
template <class StreamType>
class AnsiStreamWrapper
{
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

    static_assert(
        sp::traits::isOutStream<StreamType>::value,
        "Must be an output stream"
    );

    typedef std::ostream & (*Manipulator)(std::ostream &);
    typedef StreamType InnerStream;

public:

    typedef typename StreamType::char_type char_type;
    typedef typename StreamType::int_type int_type;
    typedef typename StreamType::pos_type pos_type;
    typedef typename StreamType::off_type off_type;
    typedef typename StreamType::traits_type traits_type;


    ////////////////////////////////////////////////////////////
    /// \brief Wraps a stream type
    ///
    /// Stream args will be passed to InnerStream's constructor
    ////////////////////////////////////////////////////////////
    template <class... StreamArgs>
    AnsiStreamWrapper(bool enableAnsi, StreamArgs &&... args)
        : inner{std::forward<StreamArgs>(args)...}, areSequencesEnabled{enableAnsi}
    {
    }

    // Move are not implemented since streams are not typically movable
    // (ie ostreams does not define moves, but stringstream does)

    ~AnsiStreamWrapper() = default;


    ////////////////////////////////////////////////////////////
    /// \brief Access the inner stream
    ///
    ////////////////////////////////////////////////////////////
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

    ////////////////////////////////////////////////////////////
    /// \brief Access the inner stream
    ///
    ////////////////////////////////////////////////////////////
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

    ////////////////////////////////////////////////////////////
    /// \brief tells if AnsiEscapes are enabled on this stream
    ///
    ////////////////////////////////////////////////////////////
    [[nodiscard]] bool
    isAnsiEnabled() const
    {
        return areSequencesEnabled;
    }

    ////////////////////////////////////////////////////////////
    /// \brief enables or disables AnsiEscapes on this stream
    ///
    ////////////////////////////////////////////////////////////
    void
    enableAnsi(bool on = true)
    {
        areSequencesEnabled = on;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Streams AnsiEscapes when they are enabled, otherwise this is a no-op
    ///
    ////////////////////////////////////////////////////////////
    template <class T, std::enable_if_t<sp::traits::isAnsiEscape<T>::value, bool> = true>
    friend AnsiStreamWrapper &
    operator<<(AnsiStreamWrapper & stream, T && seq)
    {
        if (stream.isAnsiEnabled())
            stream.stream() << seq;

        return stream;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Streams any object that is not an AnsiEscapes
    ///
    ////////////////////////////////////////////////////////////
    template <class T, std::enable_if_t<!sp::traits::isAnsiEscape<T>::value, bool> = true>
    friend AnsiStreamWrapper &
    operator<<(AnsiStreamWrapper & stream, T && obj)
    {
        stream.stream() << obj;
        return stream;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Streams io manipulators
    ///
    ////////////////////////////////////////////////////////////
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

template <class Stream, class = void>
struct AnsiWrap
{
    typedef sp::AnsiStreamWrapper<Stream> Wrapped;
};

// checking for the .stream() method may not be a good enough requirement.
template <class Stream>
struct AnsiWrap<Stream, sp::traits::void_t<decltype(std::declval<Stream>().stream())>>
{
    typedef Stream Wrapped;
};

} // namespace traits

////////////////////////////////////////////////////////////
/// \ingroup Logging
/// \brief Wraps a Stream into an ansi aware stream if it isn't already
///
////////////////////////////////////////////////////////////
template <class Stream>
using AnsiWrapped_t = typename sp::traits::AnsiWrap<Stream>::Wrapped;


////////////////////////////////////////////////////////////
/// \ingroup Logging
/// \brief Returns true if the file represents a terminal supporting ansi sequences
///
////////////////////////////////////////////////////////////
[[nodiscard]] bool
supportsAnsi(FILE * file);


////////////////////////////////////////////////////////////
/// \ingroup Logging
/// \brief Defines how ansi should be enabled
///
////////////////////////////////////////////////////////////
enum class ansiMode
{
    always,    ///< Ansi escapes are always enabled
    automatic, ///< Ansi escapes are enabled if supportsAnsi(file) is true
    never      ///< Ansi escapes are never enabled
};


////////////////////////////////////////////////////////////
/// \ingroup Logging
/// \brief Makes an ansi aware stream from a FILE*
///
/// This is mainly used to provide sp::ansiOut and sp::ansiErr
/// alternatives to std::cout and std::cerr.
///
/// Allows automatically detecting if ansi escapes should be filtered out or not
/// depending on the file used.
///
////////////////////////////////////////////////////////////
class SPIRIT_API AnsiFileStream
    : public sp::AnsiStreamWrapper<sp::details::FileStream>
{
    // TODO: Provide a way to make it streams unbuffered (sync with stdio)
    // TODO: template<charType>
    // TODO: Input sequences..? https://en.wikipedia.org/wiki/ANSI_escape_code#Terminal_input_sequences

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
        setAnsiMode(mode);
    }

    void
    setAnsiMode(ansiMode mode)
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

private:
    // use setAnsiMode instead
    using Wrapper::enableAnsi;
};

extern SPIRIT_API sp::AnsiFileStream ansiOut;
extern SPIRIT_API sp::AnsiFileStream ansiErr;


} // namespace sp


#endif // SPIRIT_ANSISTREAM_HPP
