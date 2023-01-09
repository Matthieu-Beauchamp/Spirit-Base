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


#ifndef SPIRIT_ANSIESCAPEIMPL_HPP
#define SPIRIT_ANSIESCAPEIMPL_HPP

#include "SPIRIT/Base/Concepts/Concepts.hpp"
#include "SPIRIT/Base/Configuration/config.hpp"

#include <cmath>
#include <iostream>
#include <sstream>

namespace sp
{

////////////////////////////////////////////////////////////
/// \brief Base class of Ansi terminal escape sequences
///
/// The main motivation of this class is to allow output of
/// colored text on terminals. Other functionality is also provided,
/// like text styling (bold, underlined, etc.). Eventually, more
/// generic sequences may be available.
///
/// Terminal capabilities are not queried at the time of this writing.
/// The operator<< of AnsiSequences will always output its sequence to the
/// stream. To ensure that you are not writing these sequences to a text file,
/// use the AnsiFileStream class which will determine if escapes should be ignored.
/// For c++ streams, this is impossible to determine reliably, you will need
/// to specified it to the constructor of the AnsiFileStream.
///
/// Note that the equivalent AnsiStreams of cout and cerr are provided which
/// cannot be redirected (which is one of the reason why it is impossible
/// to determine if a stream should be outputting colors).
///
////////////////////////////////////////////////////////////
class SPIRIT_API AnsiEscape
{
public:

    static constexpr const char ESC   = '\x1b';
    static constexpr const char * CSI = "\x1b[";
};

class SPIRIT_API TextStyle : public AnsiEscape
{
public:

    static constexpr const char * start = CSI;
    static constexpr const char end     = 'm';

protected:

    // codes are actually in the [ 0x20 , 0x7E ] range,
    // we use int for proper output

    // Note that these are defined for color codes
    static std::ostream &
    putCode(std::ostream & os, sp::Int32 code)
    {
        return os << CSI << code << end;
    }


    template <sp::Int32 nCodes>
    static std::ostream &
    putCodes(std::ostream & os, sp::Int32 (&codes)[nCodes])
    {
        os << CSI;
        for (sp::Int32 i = 0; i < nCodes - 1; ++i) os << codes[i] << ";";

        os << codes[nCodes - 1] << end;
        return os;
    }
};

class SPIRIT_API TerminalControl : public AnsiEscape
{
};


namespace traits
{


////////////////////////////////////////////////////////////
/// \ingroup Concepts
/// \brief All objects that derive from AnsiEscape
///
////////////////////////////////////////////////////////////
template <class... Ts>
struct isAnsiEscape : public areOfBaseType<sp::AnsiEscape, Ts...>
{
};

////////////////////////////////////////////////////////////
/// \ingroup Concepts
/// \brief All objects that derive from TextStyle
///
/// Their sequences are always in the form
/// CSI [...] m
///
////////////////////////////////////////////////////////////
template <class... Ts>
struct isTextStyle : public areOfBaseType<sp::TextStyle, Ts...>
{
};

////////////////////////////////////////////////////////////
/// \ingroup Concepts
/// \brief All objects that derive from TerminalControl
///
////////////////////////////////////////////////////////////
template <class... Ts>
struct isTerminalControl : public areOfBaseType<sp::TerminalControl, Ts...>
{
};


template <class... Args>
using EscapeType = typename sp::traits::
    Bases<sp::AnsiEscape, sp::TextStyle, sp::TerminalControl>::DeepestOf<Args...>;

} // namespace traits


namespace details
{

enum class ansiColorTarget : sp::Int32
{
    text       = 3,
    background = 4
};

template <ansiColorTarget target>
class AnsiColor : TextStyle
{
public:

    enum color : sp::Int32
    {
        black   = 0,
        red     = 1,
        green   = 2,
        yellow  = 3,
        blue    = 4,
        magenta = 5,
        cyan    = 6,
        white   = 7,

        defaultCol = 9
    };


    constexpr AnsiColor(color c) : c{c} {}


    [[nodiscard]] constexpr color
    getColor() const
    {
        return c;
    }


    [[nodiscard]] constexpr ansiColorTarget
    getTarget() const
    {
        return t;
    }

    friend std::ostream &
    operator<<(std::ostream & os, AnsiColor color)
    {
        return TextStyle::putCode(os, (sp::Int32)color.t * 10 + color.c);
    }


private:

    static constexpr ansiColorTarget t{target};
    color c;
};


template <ansiColorTarget target>
class AnsiRgbColor : TextStyle
{
public:

    // Uint16 to avoid having to cast down, values must be between 0 and 255
    constexpr AnsiRgbColor(sp::Uint16 r, sp::Uint16 g, sp::Uint16 b)
        : r{static_cast<sp::Uint8>(r)}, g{static_cast<sp::Uint8>(g)},
          b{static_cast<sp::Uint8>(b)}
    {
    }

    sp::Uint8 r;
    sp::Uint8 g;
    sp::Uint8 b;


    friend std::ostream &
    operator<<(std::ostream & os, AnsiRgbColor c)
    {
        sp::Int32 codes[5]{
            (sp::Int32)t * 10 + c.declareRgbColor,
            c.declareRgbSequence,
            c.r,
            c.g,
            c.b};

        return TextStyle::putCodes(os, codes);
    }


private:

    static constexpr sp::Int32 declareRgbColor    = 8;
    static constexpr sp::Int32 declareRgbSequence = 2;
    static constexpr ansiColorTarget t{target};
};


template <ansiColorTarget t>
class AnsiGradient : TextStyle
{
public:

    AnsiGradient(
        std::string_view text,
        AnsiRgbColor<t> start,
        AnsiRgbColor<t> end,
        bool resetAfter = true
    )
        : txt{}
    {
        std::string buf{};
        buf.reserve(
            // rgb color escapes: "\esc[38;2;r;g;bm"
            // => counting 3 digits for colors gives 19 + char => 20 per
            // original char we add the reset: \esc[39m => 5 chars
            text.size() * 20 + 5
        );
        std::stringstream ss{std::move(buf)};

        // We can't use our Math lib since this is part of core,
        // thus we do our own here

        // Conversion is required to avoid overflows since rgb is stored as Uint8
        sp::Int16 first[3]{start.r, start.g, start.b};
        sp::Int16 last[3]{end.r, end.g, end.b};

        float step[3]{};
        std::size_t nSteps = text.size() - 1;
        for (sp::Int32 i = 0; i < 3; ++i)
        {
            float deltaCol = static_cast<float>(last[i] - first[i]);
            step[i]        = deltaCol / nSteps;
        }

        for (std::size_t i = 0; i < text.size(); ++i)
        {
            AnsiRgbColor<t> col{
                static_cast<sp::Uint16>(std::round(step[0] * i + first[0])),
                static_cast<sp::Uint16>(std::round(step[1] * i + first[1])),
                static_cast<sp::Uint16>(std::round(step[2] * i + first[2]))};

            ss << col << text[i];
        }

        if (resetAfter)
            ss << AnsiColor<t>{AnsiColor<t>::defaultCol};

        this->txt = ss.str();
    }


    friend std::ostream &
    operator<<(std::ostream & os, const AnsiGradient & grad)
    {
        return os << grad.txt;
    }

private:

    std::string txt;
};


// TODO: Add the nots, make a combined modificators reset
class SPIRIT_API AnsiStyle : TextStyle
{
public:

    /// I recommend only those marked with a '(*)',
    /// as they are more likely to be supported
    enum style : sp::Int32
    {
        reset     = 0, ///< (*) This is a complete reset (including colors)
        bold      = 1, ///< (*)
        faint     = 2,
        italic    = 3,
        underline = 4, ///< (*)

        // Not widely supported
        slowBlink = 5,
        fastBlink = 6,

        swapColors = 7, ///< (*) inverse the foreground and background colors
        conceal    = 8, ///< Not widely supported
        crossed    = 9, ///< (*)

        // Your terminal may not offer these
        defaultFont = 10,
        font1       = 11,
        font2       = 12,
        font3       = 13,
        font4       = 14,
        font5       = 15,
        font6       = 16,
        font7       = 17,
        font8       = 18,
        font9       = 19,

        gothic = 20,

        doubleUnderline = 21,

        // rest is rarely available / implemented on terminals
        // so it is not provided here

    };


    constexpr AnsiStyle(style mod) : mod{mod} {}


    friend std::ostream &
    operator<<(std::ostream & os, AnsiStyle mod)
    {
        return TextStyle::putCode(os, mod.mod);
    }


private:

    style mod;
};


// These are still considered AnsiSequences since
// outputting bells or backspaces into a file is undesirable
template <char code>
class AsciiCode : TerminalControl
{
public:

    friend std::ostream &
    operator<<(std::ostream & os, AsciiCode c)
    {
        return os << code;
    };
};


// absolute positionning (1, 1) is the default, topleft corner
class CursorMoveAbs : TerminalControl
{
public:

    constexpr CursorMoveAbs(int line = 1, int column = 1)
        : line{line}, column{column}
    {
    }

    friend std::ostream &
    operator<<(std::ostream & os, CursorMoveAbs c)
    {
        return os << c.CSI << c.line << ";" << c.column << "H";
    };

private:

    int line;
    int column;
};


template <char symbol>
class CursorMoveRel : TerminalControl
{
public:

    constexpr CursorMoveRel(int n) : n{n} {}

    friend std::ostream &
    operator<<(std::ostream & os, CursorMoveRel c)
    {
        return os << c.CSI << c.n << symbol;
    };

private:

    int n;
};


template <int num, char symbol>
class Erase : TerminalControl
{
public:

    constexpr Erase() {}

    friend std::ostream &
    operator<<(std::ostream & os, Erase c)
    {
        return os << c.CSI << num << symbol;
    };

private:

    int n;
};

} // namespace details


} // namespace sp


#endif // SPIRIT_ANSIESCAPEIMPL_HPP
