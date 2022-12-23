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


#ifndef SPIRIT_ANSISEQUENCE_HPP
#define SPIRIT_ANSISEQUENCE_HPP

#include "SPIRIT/config.hpp"

#include <iostream>
#include <string>
#include <type_traits>

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
class SPIRIT_API AnsiSequence
{
public:

    static constexpr const char ESC   = '\x1b';
    static constexpr const char * CSI = "\x1b[";
    static constexpr const char end   = 'm';

protected:

    // codes are actually in the [ 0x20 , 0x7E ] range,
    // we use int for proper output

    // Note that these are defined for color codes
    static std::ostream &
    putCode(std::ostream & os, sp::Int32 code);


    template <sp::Int32 nCodes>
    static std::ostream &
    putCodes(std::ostream & os, sp::Int32 (&codes)[nCodes]);


    static std::ostream &
    reset(std::ostream & os);
};

namespace traits
{

////////////////////////////////////////////////////////////
/// \ingroup Concepts
/// \brief All objects that derive from AnsiSequence
///
////////////////////////////////////////////////////////////
template <class T>
struct isAnsiSequence
    : public std::integral_constant<
          bool,
          std::is_base_of<
              sp::AnsiSequence,
              std::remove_reference_t<std::remove_cv_t<T>>>::value>
{
};

} // namespace traits


enum class SPIRIT_API ansiColorTarget : sp::Int32
{
    text       = 3,
    background = 4
};


// TODO: make the color a template parameter? annoying if you ever want to loop
//      but allows typedefs for colors
template <ansiColorTarget target>
class SPIRIT_API AnsiColor : AnsiSequence
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


    template <ansiColorTarget t>
    friend std::ostream &
    operator<<(std::ostream & os, AnsiColor<t> color);


private:

    static constexpr ansiColorTarget t{target};
    color c;
};


typedef AnsiColor<ansiColorTarget::text> FgColor;
typedef AnsiColor<ansiColorTarget::background> BgColor;

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


template <ansiColorTarget target>
class SPIRIT_API AnsiRgbColor : AnsiSequence
{
public:

    constexpr AnsiRgbColor(sp::Uint8 r, sp::Uint8 g, sp::Uint8 b)
        : _r{r}, _g{g}, _b{b}
    {
    }


    template <ansiColorTarget t>
    friend std::ostream &
    operator<<(std::ostream & os, AnsiRgbColor<t> c);


    [[nodiscard]] sp::Uint8
    r() const;


    [[nodiscard]] sp::Uint8
    g() const;


    [[nodiscard]] sp::Uint8
    b() const;


private:

    static constexpr sp::Int32 declareRgbColor    = 8;
    static constexpr sp::Int32 declareRgbSequence = 2;
    static constexpr ansiColorTarget t{target};

    sp::Uint8 _r;
    sp::Uint8 _g;
    sp::Uint8 _b;
};


typedef AnsiRgbColor<ansiColorTarget::text> RgbFgColor;
typedef AnsiRgbColor<ansiColorTarget::background> RgbBgColor;


// TODO: Add the nots, make a combined modificators reset
class SPIRIT_API AnsiStyle : AnsiSequence
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
    operator<<(std::ostream & os, AnsiStyle mod);


private:

    style mod;
};


constexpr AnsiStyle reset{AnsiStyle::reset};
constexpr AnsiStyle bold{AnsiStyle::bold};
constexpr AnsiStyle faint{AnsiStyle::faint};
constexpr AnsiStyle italic{AnsiStyle::italic};
constexpr AnsiStyle underline{AnsiStyle::underline};
constexpr AnsiStyle slowBlink{AnsiStyle::slowBlink};
constexpr AnsiStyle fastBlink{AnsiStyle::fastBlink};
constexpr AnsiStyle swapColors{AnsiStyle::swapColors};
constexpr AnsiStyle conceal{AnsiStyle::conceal};
constexpr AnsiStyle crossed{AnsiStyle::crossed};


SPIRIT_API std::string
colorOutput(
    const std::string & txt,
    FgColor txtCol,
    BgColor bgCol,
    bool resetColorsAfter = true
);


SPIRIT_API std::string
colorFg(const std::string & txt, FgColor txtCol, bool resetColorsAfter = true);


SPIRIT_API std::string
colorBg(const std::string & txt, BgColor bgCol, bool resetColorsAfter = true);


template <ansiColorTarget t>
SPIRIT_API std::string
textGradient(
    const std::string & txt,
    AnsiRgbColor<t> firstChar,
    AnsiRgbColor<t> lastChar,
    bool resetColorAfter = true
);


SPIRIT_API std::string
modify(const std::string & txt, AnsiStyle mod, bool resetAfter = true);


SPIRIT_API std::string
modify(
    const std::string & txt,
    std::initializer_list<AnsiStyle> mods,
    bool resetAfter = true
);


////////////////////////////////////////////////////////////
// Generic Sequences
////////////////////////////////////////////////////////////

namespace details
{
template <char code>
class AsciiCode : AnsiSequence
{
public:

    friend std::ostream &
    operator<<(std::ostream & os, AsciiCode c)
    {
        return os << code;
    };
};
} // namespace details

class Bell : public details::AsciiCode<'\a'>
{
};

class Backspace : public details::AsciiCode<'\b'>
{
};

// Useful for overwriting lines (such as / - \ | loops in loaders or progress
// bars), ex: ostream << sp::EraseLine << sp::CarriageReturn << "Updated Text";
class CarriageRet : public details::AsciiCode<'\r'>
{
};

////////////////////////////////////////////////////////////
// Cursor movement
////////////////////////////////////////////////////////////

// absolute positionning (1, 1) is the default, topleft corner
class CursorMoveAbs : AnsiSequence
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

namespace details
{

template <char symbol>
class CursorRelMove : AnsiSequence
{
public:

    constexpr CursorRelMove(int n) : n{n} {}

    friend std::ostream &
    operator<<(std::ostream & os, CursorRelMove c)
    {
        return os << c.CSI << c.n << symbol;
    };

private:

    int n;
};
} // namespace details

class CursorUp : public details::CursorRelMove<'A'>
{
public:

    constexpr CursorUp(int nLines = 1) : CursorRelMove{nLines} {}
};

class CursorDown : public details::CursorRelMove<'B'>
{
public:

    constexpr CursorDown(int nLines = 1) : CursorRelMove{nLines} {}
};

class CursorRight : public details::CursorRelMove<'C'>
{
public:

    constexpr CursorRight(int nColumns = 1) : CursorRelMove{nColumns} {}
};

class CursorLeft : public details::CursorRelMove<'D'>
{
public:

    constexpr CursorLeft(int nColumns = 1) : CursorRelMove{nColumns} {}
};

class CursorToColumn : public details::CursorRelMove<'G'>
{
public:

    constexpr CursorToColumn(int col = 1) : CursorRelMove{col} {}
};


////////////////////////////////////////////////////////////
// Erasing
////////////////////////////////////////////////////////////

namespace details
{

template <int num, char symbol>
class Erase : AnsiSequence
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

class EraseScreen : public details::Erase<2, 'J'>
{
public:

    constexpr EraseScreen() {}
};

class EraseCursorToEndLine : public details::Erase<0, 'K'>
{
public:

    constexpr EraseCursorToEndLine() {}
};

class EraseStartLineToCursor : public details::Erase<1, 'K'>
{
public:

    constexpr EraseStartLineToCursor() {}
};

class EraseLine : public details::Erase<2, 'K'>
{
public:

    constexpr EraseLine() {}
};

} // namespace sp


#include "AnsiSequence_inl.hpp"

#endif // SPIRIT_ANSISEQUENCE_HPP
