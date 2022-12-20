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

#include <iostream>

#include "SPIRIT/Concepts/Concepts.hpp"
#include "SPIRIT/config.hpp"

namespace sp
{


class ColoredStream : public std::ostream
{
public:
    enum colorMode
    {
        always,
        automatic,
        never
    };

    ColoredStream(std::ostream & os, colorMode mode = automatic);

    void setColorMode();

    std::ostream &
    inner();

    template <class T, std::enable_if_t<sp::Printable<T>::value, bool> = true>
    ColoredStream &
    operator<<(const T & obj);

    template <class T, std::enable_if_t<sp::Printable<T>::value, bool> = true>
    ColoredStream &
    operator<<(T & obj);


private:
    // forbid redirecting output
    // TODO: Or force the user to specify a new color mode (otherwise it's very bug prone)
    using std::ostream::rdbuf;

    std::ostream & os;

    colorMode mode;
    bool shouldColor = false;

    bool
    isConsoleStream();

#ifdef SPIRIT_OS_WINDOWS
    bool enableVirtualTerminal() const;
#endif

};


} // namespace sp


#endif // SPIRIT_ENGINE_COLOREDSTREAM_HPP
