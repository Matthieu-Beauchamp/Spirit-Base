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


#ifndef SPIRIT_ANSIESCAPE_HPP
#define SPIRIT_ANSIESCAPE_HPP


#include "details/AnsiEscapeImpl.hpp"
#include "Format.hpp"

#include <iostream>
#include <tuple>
#include <type_traits>

namespace sp
{

////////////////////////////////////////////////////////////
/// \ingroup Logging
/// \defgroup AnsiEscapes AnsiEscapes
/// \brief ansi escapes are printables terminal commands
/// 
/// They allow control the style of outputting text as well 
/// as performing more generic terminal control 
/// (like written over a previously written line).
/// 
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
/// \ingroup AnsiEscapes
/// \brief Converts any AnsiEscape to a string
/// 
////////////////////////////////////////////////////////////
template <class Esc, std::enable_if_t<sp::traits::isAnsiEscape<Esc>::value, bool> = true>
std::string
toStr(Esc && esc)
{
    return sp::format(esc);
}

////////////////////////////////////////////////////////////
/// \ingroup AnsiEscapes
/// \brief Combines multiples AnsiEscape together
/// 
/// When streamed, the contained escapes are streamed in the order they
/// are given in the constructor.
/// 
/// The escape type will properly be detected as AnsiEscape, TextStyle
/// or TerminalControl according to the escapes it contains.
/// 
////////////////////////////////////////////////////////////
template <class... Args>
class Escapes : traits::EscapeType<Args...>
{
    // TODO: MSVC gives a bunch of warning saying that std::tuple needs to have
    //  dll interface for clients of ... to be used

public:
    template<class... CtorArgs>
    constexpr Escapes(CtorArgs&&... args) : tup{std::forward<CtorArgs>(args)...}{}

    friend std::ostream &
    operator<<(std::ostream & os, const Escapes & e)
    {
        return ((os << std::get<Args>(e.tup)), ...);
    }

private:
    std::tuple<Args...> tup;

};

// deduction
template <class... Args>
Escapes(Args &&...) -> Escapes<Args...>;



////////////////////////////////////////////////////////////
/// \ingroup AnsiEscapes
/// \defgroup TextStyles TextStyles
/// \brief Subset of ansi escapes that defines the style of outputted text
/// \{
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// \brief Defines a foreground (text) color
/// 
////////////////////////////////////////////////////////////
SPIRIT_API typedef details::AnsiColor<details::ansiColorTarget::text> FgColor;

////////////////////////////////////////////////////////////
/// \brief Defines a background color
/// 
////////////////////////////////////////////////////////////
SPIRIT_API typedef details::AnsiColor<details::ansiColorTarget::background> BgColor;

constexpr FgColor black{FgColor::black};
constexpr FgColor red{FgColor::red};
constexpr FgColor green{FgColor::green};
constexpr FgColor yellow{FgColor::yellow};
constexpr FgColor blue{FgColor::blue};
constexpr FgColor magenta{FgColor::magenta};
constexpr FgColor cyan{FgColor::cyan};
constexpr FgColor white{FgColor::white};
constexpr FgColor defaultFg{FgColor::defaultCol};

constexpr BgColor onBlack{BgColor::black};
constexpr BgColor onRed{BgColor::red};
constexpr BgColor onGreen{BgColor::green};
constexpr BgColor onYellow{BgColor::yellow};
constexpr BgColor onBlue{BgColor::blue};
constexpr BgColor onMagenta{BgColor::magenta};
constexpr BgColor onCyan{BgColor::cyan};
constexpr BgColor onWhite{BgColor::white};
constexpr BgColor onDefault{BgColor::defaultCol};


////////////////////////////////////////////////////////////
/// \brief Defines a color with Rgb codes
/// 
/// This is less widely supported than the predefined FgColor and BgColor
/// 
////////////////////////////////////////////////////////////
template <details::ansiColorTarget t>
class RgbColor : public details::AnsiRgbColor<t>
{
public:

    constexpr RgbColor(sp::Uint8 r, sp::Uint8 g, sp::Uint8 b)
        : details::AnsiRgbColor<t>{r, g, b}
    {
    }

    friend std::ostream &
    operator<<(std::ostream & os, RgbColor c)
    {
        return os << static_cast<details::AnsiRgbColor<t>>(c);
    }
};

SPIRIT_API typedef RgbColor<details::ansiColorTarget::text> RgbFgColor;
SPIRIT_API typedef RgbColor<details::ansiColorTarget::background> RgbBgColor;

////////////////////////////////////////////////////////////
/// \brief Uses RgbColor to apply a color gradient to text
/// 
////////////////////////////////////////////////////////////
template <details::ansiColorTarget t>
class Gradient : public details::AnsiGradient<t>
{
public:

    Gradient(
        std::string_view text,
        RgbColor<t> start,
        RgbColor<t> end,
        bool resetAfter = true
    )
        : details::AnsiGradient<t>{text, start, end, resetAfter}
    {
    }

    friend std::ostream &
    operator<<(std::ostream & os, const Gradient & g)
    {
        return os << static_cast<const details::AnsiGradient<t> &>(g);
    };
};

SPIRIT_API typedef Gradient<details::ansiColorTarget::text> FgGradient;
SPIRIT_API typedef Gradient<details::ansiColorTarget::background> BgGradient;


////////////////////////////////////////////////////////////
/// \brief Styles text output
/// 
////////////////////////////////////////////////////////////
SPIRIT_API typedef details::AnsiStyle Style;

constexpr Style reset{Style::reset};
constexpr Style bold{Style::bold};
constexpr Style faint{Style::faint};
constexpr Style italic{Style::italic};
constexpr Style underline{Style::underline};
constexpr Style slowBlink{Style::slowBlink};
constexpr Style fastBlink{Style::fastBlink};
constexpr Style swapColors{Style::swapColors};
constexpr Style conceal{Style::conceal};
constexpr Style crossed{Style::crossed};

/// \}


////////////////////////////////////////////////////////////
/// \ingroup AnsiEscapes
/// \defgroup TerminalControls TerminalControls
/// \brief Ansi Sequences for generic terminal commands
/// \{
////////////////////////////////////////////////////////////

SPIRIT_API typedef details::AsciiCode<'\a'> Bell;
SPIRIT_API typedef details::AsciiCode<'\b'> Backspace;
SPIRIT_API typedef details::AsciiCode<'\r'> CarriageRet;


////////////////////////////////////////////////////////////
/// \brief Absolute movement of the cursor
/// TODO: Detail behavior
////////////////////////////////////////////////////////////
class MoveCursorTo : public details::CursorMoveAbs
{
public:

    // TODO: Use unsigned
    constexpr MoveCursorTo(int line = 1, int column = 1)
        : details::CursorMoveAbs{line, column}
    {
    }

    friend std::ostream &
    operator<<(std::ostream & os, MoveCursorTo mov)
    {
        return os << static_cast<details::CursorMoveAbs>(mov);
    };
};


////////////////////////////////////////////////////////////
/// \brief Relative movement of the cursor
/// 
////////////////////////////////////////////////////////////
template <char sym>
class MoveCursor : public details::CursorMoveRel<sym>
{
public:

    // TODO: Use unsigned
    constexpr MoveCursor(int n) : details::CursorMoveRel<sym>{n} {}

    friend std::ostream &
    operator<<(std::ostream & os, MoveCursor mov)
    {
        return os << static_cast<details::CursorMoveRel<sym>>(mov);
    };
};

SPIRIT_API typedef MoveCursor<'A'> CursorUp;
SPIRIT_API typedef MoveCursor<'B'> CursorDown;
SPIRIT_API typedef MoveCursor<'C'> CursorRight;
SPIRIT_API typedef MoveCursor<'D'> CursorLeft;
SPIRIT_API typedef MoveCursor<'G'> CursorToColumn;


////////////////////////////////////////////////////////////
/// \brief Erasing functionnality
/// 
/// used with carriage return, allows overwritting lines,
/// see AnsiCodes.cpp in examples.
/// 
////////////////////////////////////////////////////////////
template <int num, char sym>
using Erase = details::Erase<num, sym>;

typedef Erase<2, 'J'> EraseScreen;
typedef Erase<0, 'K'> EraseCursorToEndLine;
typedef Erase<1, 'K'> EraseStartLineToCursor;
typedef Erase<2, 'K'> EraseLine;

/// \}

} // namespace sp


#endif // SPIRIT_ANSIESCAPE_HPP
