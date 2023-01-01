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


#ifndef SPIRIT_ENGINE_COLOREDSTREAM_HPP
#define SPIRIT_ENGINE_COLOREDSTREAM_HPP

#include "AnsiEscape.hpp"
#include "SPIRIT/Concepts/Concepts.hpp"
#include "SPIRIT/config.hpp"
#include "details/FileBuf.hpp"

#include <memory>
#include <sstream>

namespace sp
{

template <class StreamType>
class NoRedirect : public StreamType
{
public:

    typedef typename StreamType::char_type char_type;
    typedef typename StreamType::int_type int_type;
    typedef typename StreamType::pos_type pos_type;
    typedef typename StreamType::off_type off_type;
    typedef typename StreamType::traits_type traits_type;

    using StreamType::StreamType;

private:
    using StreamType::rdbuf;
};


////////////////////////////////////////////////////////////
/// It is necessary that we do not inherit from ostream
/// since it causes ambiguous overload resolutions for operator<<.
/// Furthermore, since our AnsiSequences rely on being detected by
/// a stream which knows whether or not it supports them, they always output when
/// their overloaded operator<< is called.
/// This means that when the inherited operator<< is called, an ostream& is
/// returned and every subsequent AnsiEscape is passed without regards to the ansiStream.
////////////////////////////////////////////////////////////


// Usage is intended to create stringstreams which will behave according to
//  an AnsiStream's escape sequences support.
// ie AnsiStream::makeStream<std::stringstream>(std::stringstream{}) -> AnsiStreamWrapper
template <class StreamType> //,
                            //   std::enable_if_t<sp::traits::isStream<StreamType>::value, bool> = true>
class AnsiStreamWrapper
{
    typedef StreamType & (*Manipulator)(StreamType &);
    typedef NoRedirect<StreamType> InnerStream;

public:

    typedef typename StreamType::char_type char_type;
    typedef typename StreamType::int_type int_type;
    typedef typename StreamType::pos_type pos_type;
    typedef typename StreamType::off_type off_type;
    typedef typename StreamType::traits_type traits_type;

    AnsiStreamWrapper(InnerStream && inner, bool enableAnsi = false)
        : inner{inner}, areSequencesEnabled{enableAnsi}
    {
    }

    template <class... Args>
    AnsiStreamWrapper(bool enableAnsi, Args &&... args)
        : inner{std::forward<Args>(args)...}, areSequencesEnabled{enableAnsi}
    {
    }


    ~AnsiStreamWrapper() = default;

    // TODO: Only if StreamType is movable, can't use enable if...
    // AnsiStreamWrapper(AnsiStreamWrapper && other) { *this = std::move(other); }
    // AnsiStreamWrapper &
    // operator=(AnsiStreamWrapper && other)
    // {
    //     std::swap(
    //         static_cast<StreamType &>(inner),
    //         static_cast<StreamType &>(other.inner)
    //     );
    //     areSequencesEnabled = other.areSequencesEnabled;
    //     return *this;
    // }

    // Allows access to stream specific behavior,
    // cannot prevent users from changing the streambuf...
    InnerStream &
    stream()
    {
        return inner;
    }

    const InnerStream &
    stream() const
    {
        return inner;
    }

    InnerStream *
    operator->()
    {
        return std::addressof(inner);
    }

    bool
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
        if (stream.areSequencesEnabled)
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
    // modifies to behavior of an existing stream object
    InnerStream inner;
};


template <class StreamType, class... Args>
auto
makeAnsiAwareStream(bool ansiEnabled, Args &&... args)
{
    typedef sp::AnsiStreamWrapper<StreamType> WrappedStream;

    return WrappedStream{ansiEnabled, std::forward<Args>(args)...};
}


bool
supportsAnsi(FILE * file);


// TODO: Provide a way to make it streams unbuffered (sync with stdio)
// TODO: template<charType>
// TODO: Input sequences..? https://en.wikipedia.org/wiki/ANSI_escape_code#Terminal_input_sequences
class SPIRIT_API AnsiStream : public sp::AnsiStreamWrapper<std::ostream>
{
    typedef sp::AnsiStreamWrapper<std::ostream> Wrapper;
    typedef std::ostream StreamType;

public:

    typedef typename Wrapper::char_type char_type;
    typedef typename Wrapper::int_type int_type;
    typedef typename Wrapper::pos_type pos_type;
    typedef typename Wrapper::off_type off_type;
    typedef typename Wrapper::traits_type traits_type;


    enum sequenceMode
    {
        always,
        automatic,
        never
    };

    AnsiStream(FILE * file, sequenceMode mode = automatic)
        : Wrapper{false, std::addressof(fileBuf)}, fileBuf{file}
    {
        changeSequenceMode(mode);
    }

    // See AnsiStreamWrapper
    // AnsiStream(AnsiStream && other, sequenceMode mode = automatic)
    //     : Wrapper{static_cast<Wrapper &&>(other)}, fileBuf{other.file()}
    // {
    //     changeSequenceMode(mode);
    // }

    ~AnsiStream() = default;

    [[nodiscard]] FILE *
    file() const
    {
        return fileBuf.file();
    }

    template <class ChildStreamType, class... Args>
    [[nodiscard]] auto
    makeAnsiChildStream(Args &&... args) const
    {
        return makeAnsiAwareStream<ChildStreamType>(
            isAnsiEnabled(),
            std::forward<Args>(args)...
        );
    }

    [[nodiscard]] auto
    makeStringStream() const
    {
        return makeAnsiChildStream<std::stringstream>();
    }

private:

    sp::details::OutFileBuf fileBuf;

    void
    changeSequenceMode(sequenceMode mode)
    {
        switch (mode)
        {
        case always: enableAnsi(true); return;
        case never: enableAnsi(false); return;
        case automatic: enableAnsi(supportsAnsi(fileBuf.file())); return;
        }
    }
};

extern SPIRIT_API sp::AnsiStream ansiOut;
extern SPIRIT_API sp::AnsiStream ansiErr;


} // namespace sp


#endif // SPIRIT_ENGINE_COLOREDSTREAM_HPP
