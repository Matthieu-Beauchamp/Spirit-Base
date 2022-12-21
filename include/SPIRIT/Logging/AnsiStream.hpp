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

#include "SPIRIT/Concepts/Concepts.hpp"
#include "SPIRIT/config.hpp"
#include "details/AnsiStreamWrapper.hpp"
#include "details/FileBuf.hpp"

#include <iostream>
#include <memory>
#include <variant>

namespace sp
{

// TODO: Provide a way to make it streams unbuffered (sync with stdio)
// TODO: template<charType>
// TODO: Input sequences..? https://en.wikipedia.org/wiki/ANSI_escape_code#Terminal_input_sequences
class SPIRIT_API AnsiStream : public sp::details::AnsiStreamWrapper<std::ostream>
{
    typedef sp::details::AnsiStreamWrapper<std::ostream> Wrapper;
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
        : fileBuf{new sp::details::OutFileBuf{file}}
    {
        this->inner = std::move(
            std::make_unique<sp::details::NoRedirect<StreamType>>(fileBuf.get())
        );
        changeSequenceMode(mode);
    }

    // template <class Stream>
    // AnsiStream(Stream && os, sequenceMode mode)
    //     : Wrapper{std::forward<Stream>(os)}
    // {
    //     static_assert(std::is_base_of<std::ostream, Stream>::value,
    //                   "Using template to avoid duplicating for different "
    //                   "references type");

    //     changeSequenceMode(mode);
    // }

    AnsiStream(AnsiStream && other, sequenceMode mode = automatic)
    {
        std::swap(inner, other.inner);
        std::swap(fileBuf, other.fileBuf);
        changeSequenceMode(mode);
    }

    ~AnsiStream() = default;

    FILE *
    file() const
    {
        return fileBuf->file();
    }

    template <class ChildStreamType, class... Args>
    [[nodiscard]] sp::details::AnsiStreamWrapper<ChildStreamType>
    makeAnsiChildStream(Args &&... args) const
    {
        typedef sp::details::AnsiStreamWrapper<ChildStreamType> WrappedStream;
        typedef sp::details::NoRedirect<ChildStreamType> InnerStream;

        WrappedStream stream{areSequencesEnabled};
        stream.inner = std::move(
            std::make_unique<InnerStream>(std::forward<Args>(args)...)
        );
        return std::move(stream);
    }

    [[nodiscard]] sp::details::AnsiStreamWrapper<std::stringstream>
    makeStringStream() const
    {
        return makeAnsiChildStream<std::stringstream>();
    }

private:

    // using std::ostream::rdbuf;


    // forbid redirecting output
    // TODO: Or force the user to specify a new color mode (otherwise it's very bug prone)
    std::unique_ptr<sp::details::OutFileBuf> fileBuf{nullptr};

    void
    changeSequenceMode(sequenceMode mode)
    {
        areSequencesEnabled = checkSequenceSupport(mode);
    }


    bool
    checkSequenceSupport(sequenceMode mode);

    bool
    enableVirtualTerminal() const;
};

extern SPIRIT_API sp::AnsiStream ansiOut;
extern SPIRIT_API sp::AnsiStream ansiErr;


} // namespace sp


#endif // SPIRIT_ENGINE_COLOREDSTREAM_HPP
