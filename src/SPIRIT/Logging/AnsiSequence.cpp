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


#include <sstream>
#include "SPIRIT/Logging/AnsiSequence.hpp"

namespace sp
{


// ////////////////////////////////////////////////////////
std::ostream &
AnsiSequence::putCode(std::ostream & os, sp::Int32 code)
{
    return os << CSI << code << end;
}


// ////////////////////////////////////////////////////////
std::ostream &
AnsiSequence::reset(std::ostream & os)
{
    return putCode(os, 0);
}


// ////////////////////////////////////////////////////////
std::ostream &
operator<<(std::ostream & os, AnsiStyle mod)
{
    return AnsiSequence::putCode(os, mod.mod);
}

// ////////////////////////////////////////////////////////
std::string
colorOutput(const std::string & txt,
            FgColor txtCol,
            BgColor bgCol,
            bool resetColorsAfter)
{
    std::stringstream ss;
    ss << txtCol << bgCol << txt;
    if (resetColorsAfter)
        ss << defaultFg << onDefault;
    return ss.str();
}

// ////////////////////////////////////////////////////////
std::string
colorFg(const std::string & txt, FgColor txtCol, bool resetColorsAfter)
{
    return colorOutput(txt, txtCol, onDefault, resetColorsAfter);
}

// ////////////////////////////////////////////////////////
std::string
colorBg(const std::string & txt, BgColor bgCol, bool resetColorsAfter)
{
    return colorOutput(txt, defaultFg, bgCol, resetColorsAfter);
}


// ////////////////////////////////////////////////////////
std::string
modify(const std::string & txt, AnsiStyle mod, bool resetAfter)
{
    std::stringstream ss{};
    ss << mod << txt;
    if (resetAfter)
        ss << reset;
    return ss.str();
}

// ////////////////////////////////////////////////////////
std::string
modify(const std::string & txt,
       std::initializer_list<AnsiStyle> mods,
       bool resetAfter)
{
    std::stringstream ss{};

    for (AnsiStyle mod : mods)
        ss << mod;

    ss << txt;
    if (resetAfter)
        ss << reset;

    return ss.str();
}



} // namespace sp
