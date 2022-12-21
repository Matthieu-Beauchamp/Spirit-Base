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


#ifndef SPIRIT_ANSISEQUENCE_INL_HPP
#define SPIRIT_ANSISEQUENCE_INL_HPP

#include <sstream>
#include <cmath>
#include "AnsiSequence.hpp"

namespace sp
{

// ////////////////////////////////////////////////////////
template <sp::Int32 nCodes>
std::ostream &
AnsiSequence::putCodes(std::ostream & os, sp::Int32 (&codes)[nCodes])
{
    os << CSI;
    for (sp::Int32 i = 0; i < nCodes - 1; ++i)
        os << codes[i] << ";";

    os << codes[nCodes - 1] << end;
    return os;
}

// ////////////////////////////////////////////////////////
template <ansiColorTarget t>
std::ostream &
operator<<(std::ostream & os, AnsiColor<t> color)
{
    return AnsiSequence::putCode(os, (sp::Int32)t * 10 + color.c);
}

// ////////////////////////////////////////////////////////
template <ansiColorTarget t>
std::ostream &
operator<<(std::ostream & os, AnsiRgbColor<t> c)
{
    sp::Int32 codes[5]{(sp::Int32)t * 10 + AnsiRgbColor<t>::declareRgbColor,
                       AnsiRgbColor<t>::declareRgbSequence,
                       c.r(),
                       c.g(),
                       c.b()};

    return AnsiSequence::putCodes(os, codes);
}

// ////////////////////////////////////////////////////////
template <ansiColorTarget t>
sp::Uint8
AnsiRgbColor<t>::r() const
{
    return _r;
}

// ////////////////////////////////////////////////////////
template <ansiColorTarget t>
sp::Uint8
AnsiRgbColor<t>::g() const
{
    return _g;
}

// ////////////////////////////////////////////////////////
template <ansiColorTarget t>
sp::Uint8
AnsiRgbColor<t>::b() const
{
    return _b;
}


// ////////////////////////////////////////////////////////
template <ansiColorTarget t>
std::string
textGradient(const std::string & txt,
             AnsiRgbColor<t> firstChar,
             AnsiRgbColor<t> lastChar,
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
        ss << AnsiRgbColor<t>{static_cast<sp::Uint8>(std::round(
                                  (float)firstChar.r() + (step[0] * i))),
                              static_cast<sp::Uint8>(std::round(
                                  (float)firstChar.g() + (step[1] * i))),
                              static_cast<sp::Uint8>(std::round(
                                  (float)firstChar.b() + (step[2] * i)))}
           << txt[i];
    }

    if (resetColorAfter)
        ss << AnsiColor<t>{AnsiColor<t>::defaultCol};

    return ss.str();
}


} // namespace sp


#endif // SPIRIT_ANSISEQUENCE_INL_HPP
