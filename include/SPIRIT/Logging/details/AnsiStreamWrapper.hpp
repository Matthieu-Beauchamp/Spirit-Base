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


#ifndef SPIRIT_ANSISTREAMWRAPPER_HPP
#define SPIRIT_ANSISTREAMWRAPPER_HPP

#include "SPIRIT/Concepts/Concepts.hpp"
#include "SPIRIT/Logging/AnsiSequence.hpp"

#include <iosfwd>
#include <memory>
#include <type_traits>

namespace sp
{
class AnsiStream;

namespace details
{

// TODO: we could wrap our streams in a struct which makes rdbuf private
// Then we provide .stream() or operator->() to access the stream (this struct)

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
    friend AnsiStream;
    using StreamType::rdbuf;
};

////////////////////////////////////////////////////////////
/// It is necessary that we do not inherit from ostream
/// since it causes ambiguous overload resolutions for operator<<.
/// Furthermore, since our AnsiSequences rely on being detected by
/// a stream which knows whether or not it supports them, they always output when
/// their overloaded operator<< is called.
/// This means that when the inherited operator<< is called, an ostream& is
/// returned and every subsequent AnsiSequence is passed without regards to the ansiStream.
////////////////////////////////////////////////////////////
// TODO: Will need to redefine methods to call the owned stream

// TODO: Manipulators won't work: https://stackoverflow.com/questions/14829770/c-wrapper-class-for-iostream-use-stream-modifiers-like-stdendl-with-operato
//  more concise https://stackoverflow.com/questions/25615253/correct-template-method-to-wrap-a-ostream

// Usage is intended to create stringstreams which will behave according to
//  an AnsiStream's escape sequences support.
// ie AnsiStream::makeStream<std::stringstream>(std::stringstream{}) -> AnsiStreamWrapper
template <class StreamType> //,
                            //   std::enable_if_t<sp::traits::isStream<StreamType>::value, bool> = true>
class AnsiStreamWrapper
{
    typedef StreamType & (*Manipulator)(StreamType &);

public:

    typedef typename StreamType::char_type char_type;
    typedef typename StreamType::int_type int_type;
    typedef typename StreamType::pos_type pos_type;
    typedef typename StreamType::off_type off_type;
    typedef typename StreamType::traits_type traits_type;

    ~AnsiStreamWrapper() = default;

    AnsiStreamWrapper(AnsiStreamWrapper && other) { *this = std::move(other); }

    AnsiStreamWrapper &
    operator=(AnsiStreamWrapper && other)
    {
        std::swap(inner, other.inner);
        areSequencesEnabled = other.areSequencesEnabled;
        return *this;
    }

    // Allows access to stream specific behavior,
    // cannot prevent users from changing the streambuf...
    NoRedirect<StreamType> &
    stream()
    {
        return *inner;
    }

    const NoRedirect<StreamType> &
    stream() const
    {
        return *inner;
    }

    NoRedirect<StreamType> *
    operator->()
    {
        return inner.get();
    }

    bool
    isAnsiEnabled() const
    {
        return areSequencesEnabled;
    }

    template <class T, std::enable_if_t<sp::traits::isAnsiSequence<T>::value, bool> = true>
    friend AnsiStreamWrapper &
    operator<<(AnsiStreamWrapper & stream, T && seq)
    {
        if (stream.areSequencesEnabled) stream.stream() << seq;

        return stream;
    }

    template <class T, std::enable_if_t<!sp::traits::isAnsiSequence<T>::value, bool> = true>
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

    friend sp::AnsiStream;

    AnsiStreamWrapper(bool enableSequences = false)
        : areSequencesEnabled{enableSequences}
    {
    }


    bool areSequencesEnabled;

    // Constructors were a mess...
    // This will be initialized by AnsiStream in its constructors.
    // We could not initialize everything at the same time, thus it is deferred.
    std::unique_ptr<NoRedirect<StreamType>> inner{nullptr};
};

} // namespace details


} // namespace sp


#endif // SPIRIT_ANSISTREAMWRAPPER_HPP
