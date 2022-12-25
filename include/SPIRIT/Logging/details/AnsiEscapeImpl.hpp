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

#include "SPIRIT/config.hpp"
#include "SPIRIT/Concepts/Concepts.hpp"

#include <cmath>
#include <iostream>

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
/// use the AnsiStream class which will determine if escapes should be ignored.
/// For c++ streams, this is impossible to determine reliably, you will need
/// to specified it to the constructor of the AnsiStream.
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


////////////////////////////////////////////////////////////
// Boilerplate, use EscapeType_t
////////////////////////////////////////////////////////////
// template <class Base0, class Base1, class Base2, class... Args>
// struct WhichBase
// {
//     static constexpr bool which[3]
//     {
//         areOfBaseType<Base0, Args...>::value,
//             areOfBaseType<Base1, Args...>::value,
//             areOfBaseType<Base2, Args...>::value
//     }
// };

// // Base tree is expected to be Base0 +-> Base1
// //                                   +-> Base2
// template <bool isBase0, bool isBase1, bool isBase2>
// struct EscapeType
// {
// };

// template<bool isBase0>
// struct EscapeType<isBase0, true, false>
// {
//     typedef 
// };




////////////////////////////////////////////////////////////
/// \ingroup Concepts
/// \brief Determine the common Escape type of Args...
///
/// If one of Args is not an AnsiEscape, no type is defined
////////////////////////////////////////////////////////////
// template <class... Args>
// using EscapeType_t = typename EscapeType<Args...>::type;

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

    template <
        class Str,
        std::enable_if_t<std::is_convertible<Str, std::string>::value, bool> = true>
    constexpr AnsiGradient(
        AnsiRgbColor<t> start,
        Str && text,
        AnsiRgbColor<t> end,
        bool resetAfter = true
    )
        : txt{std::forward<Str>(text)}, start{start}, end{end}, resetAfter{
                                                                    resetAfter}
    {
    }

    friend std::ostream &
    operator<<(std::ostream & os, const AnsiGradient & grad)
    {
        // We can't use our Math lib since this is part of core,
        // thus we do our own here

        // Conversion is required to avoid overflows since rgb is stored as Uint8
        sp::Int16 start[3]{grad.start.r(), grad.start.g(), grad.start.b()};
        sp::Int16 end[3]{grad.end.r(), grad.end.g(), grad.end.b()};

        float step[3]{};
        std::size_t nSteps = grad.txt.size() - 1;
        for (sp::Int32 i = 0; i < 3; ++i)
        {
            float deltaCol = end[i] - start[i];
            step[i]        = deltaCol / nSteps;
        }

        for (std::size_t i = 0; i < grad.txt.size(); ++i)
        {
            AnsiRgbColor<t> col{
                static_cast<sp::Uint8>(std::round(step[0] * i + start[0])),
                static_cast<sp::Uint8>(std::round(step[1] * i + start[1])),
                static_cast<sp::Uint8>(std::round(step[2] * i + start[2]))};

            os << col << grad.txt[i];
        }

        if (grad.resetAfter)
            os << AnsiColor<t>{AnsiColor<t>::defaultCol};

        return os;
    }

private:

    std::string txt;
    AnsiRgbColor<t> start;
    AnsiRgbColor<t> end;
    bool resetAfter;
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
