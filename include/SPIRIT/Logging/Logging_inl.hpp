////////////////////////////////////////////////////////////
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


#ifndef SPIRIT_ENGINE_LOGGING_INL
#define SPIRIT_ENGINE_LOGGING_INL

#include <sstream>
#include <string>

#include "SPIRIT/Logging/Logging.hpp"


namespace sp
{

namespace IO
{


// ////////////////////////////////////////////////////////
template <class... Args>
std::string
format(const std::string & str, Args &&... args)
{
    return fmt::format(fmt::runtime(str), std::forward<Args>(args)...);
}

// ////////////////////////////////////////////////////////
template <class T, std::enable_if_t<sp::Printable<T>::value, bool> = true>
std::string
format(const T & printable)
{
    // Fixes a bug (Mingw64) where when we call format with a single argument,
    //      fmt::format complains that there is no known conversion
    //      to format_string
    std::stringstream txt{};
    txt << printable;

    return txt.str();
}


// ////////////////////////////////////////////////////////
template <terminalColorTarget t>
std::ostream &
operator<<(std::ostream & os, TerminalColor<t> color)
{
    return priv::AnsiSGR::putCode(os, (sp::Int32)t * 10 + color.c);
}

// ////////////////////////////////////////////////////////
template <terminalColorTarget t>
std::ostream &
operator<<(std::ostream & os, RgbTerminalColor<t> c)
{
    sp::Int32 codes[5]{(sp::Int32)t * 10 + RgbTerminalColor<t>::declareRgbColor,
                       RgbTerminalColor<t>::declareRgbSequence,
                       c.r(),
                       c.g(),
                       c.b()};

    return priv::AnsiSGR::putCodes(os, codes);
}

// ////////////////////////////////////////////////////////
template <terminalColorTarget t>
sp::Uint8
RgbTerminalColor<t>::r() const
{
    return _r;
}

// ////////////////////////////////////////////////////////
template <terminalColorTarget t>
sp::Uint8
RgbTerminalColor<t>::g() const
{
    return _g;
}

// ////////////////////////////////////////////////////////
template <terminalColorTarget t>
sp::Uint8
RgbTerminalColor<t>::b() const
{
    return _b;
}


// ////////////////////////////////////////////////////////
template <terminalColorTarget t>
std::string
textGradient(const std::string & txt,
             RgbTerminalColor<t> firstChar,
             RgbTerminalColor<t> lastChar,
             bool resetColorAfter)
{
    // We can't use our Math lib since this is part of core,
    // thus we do our own here

    float step[3]{};
    sp::Int16 deltaCols[3]{
        (static_cast<sp::Int16>((sp::Int16)lastChar.r() - firstChar.r())),
        (static_cast<sp::Int16>((sp::Int16)lastChar.g() - firstChar.g())),
        (static_cast<sp::Int16>((sp::Int16)lastChar.b() - firstChar.b()))};

    std::size_t nChars = txt.size();
    for (sp::Int32 i = 0; i < 3; ++i)
        step[i] = (float)deltaCols[i] / (nChars - 1);

    std::stringstream ss{};
    for (std::size_t i = 0; i < nChars; ++i)
    {
        ss << RgbTerminalColor<t>{static_cast<sp::Uint8>(std::round(
                                      (float)firstChar.r() + (step[0] * i))),
                                  static_cast<sp::Uint8>(std::round(
                                      (float)firstChar.g() + (step[1] * i))),
                                  static_cast<sp::Uint8>(std::round(
                                      (float)firstChar.b() + (step[2] * i)))}
           << txt[i];
    }

    if (resetColorAfter)
        ss << TerminalColor<t>{TerminalColor<t>::defaultCol};

    return ss.str();
}

} // namespace IO

namespace priv
{

// ////////////////////////////////////////////////////////
template <sp::Int32 nCodes>
std::ostream &
AnsiSGR::putCodes(std::ostream & os, sp::Int32 (&codes)[nCodes])
{
    if (enableAnsiColor)
    {
        os << CSI;
        for (sp::Int32 i = 0; i < nCodes - 1; ++i)
            os << codes[i] << ";";

        os << codes[nCodes - 1] << end;
    }
    return os;
}

} // namespace priv


} // namespace sp


#endif // SPIRIT_ENGINE_LOGGING_INL
