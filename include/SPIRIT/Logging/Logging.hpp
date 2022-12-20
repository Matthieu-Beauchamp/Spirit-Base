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

#ifndef SPIRIT_ENGINE_LOGGING_HPP
#define SPIRIT_ENGINE_LOGGING_HPP

#include <iostream>

#include "SPIRIT/Concepts/Concepts.hpp"
#include "SPIRIT/config.hpp"


#if defined(SPIRIT_VERBOSE) || defined(SPIRIT_DEBUG)
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE


#else
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#endif

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

// must be included when defining operator<< to log with spdlog / fmt
// Templated types may require explicit formatter specialization, see below
//
// template<> // may take template args, otherwise leave empty
// struct fmt::formatter<MyType> : fmt::ostream_formatter{}
#include "spdlog/fmt/ostr.h"


namespace sp
{

////////////////////////////////////////////////////////////
/// \brief Contains our Logging / IO methods and classes
///
/// This namespace aims to remove clutter from sp:: since all of these
/// functions and constexpr entities will be included everywhere.
///
/// Also, some very common names are used. Full qualification is recommended
/// when using this namespace.
///
////////////////////////////////////////////////////////////
namespace IO
{

void
setLogsOutput(std::ostream & os);


////////////////////////////////////////////////////////////
/// \brief Format a string according to fmt::format syntax
///
/// TODO: Link
///
/// #include "spdlog/fmt/ostr.h" // must be included to log with spdlog / fmt
///     in the file where a std::ostream & operator<< (std::ostream & os, MyClass & c)
///     function or method is defined. Otherwise, it won't be able to log
///
////////////////////////////////////////////////////////////
template <class... Args>
SPIRIT_API std::string
format(const std::string & str, Args &&... args);


template <class T, std::enable_if_t<sp::Printable<T>::value, bool> = true>
SPIRIT_API std::string
format(const T & printable);


enum class SPIRIT_API terminalColorTarget : sp::Int32
{
    text       = 3,
    background = 4
};


// TODO: make the color a template parameter? annoying if you ever want to loop
//      but allows typedefs for colors
template <terminalColorTarget target>
class SPIRIT_API TerminalColor
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


    constexpr TerminalColor(color c) : c{c} {}


    [[nodiscard]] constexpr color
    getColor() const
    {
        return c;
    }


    [[nodiscard]] constexpr terminalColorTarget
    getTarget() const
    {
        return t;
    }


    template <terminalColorTarget t>
    friend std::ostream &
    operator<<(std::ostream & os, TerminalColor<t> color);


private:
    static constexpr terminalColorTarget t{target};
    color c;
};


typedef TerminalColor<terminalColorTarget::text> TextColor;
typedef TerminalColor<terminalColorTarget::background> BgColor;

constexpr TextColor black{TextColor::black};
constexpr TextColor red{TextColor::red};
constexpr TextColor green{TextColor::green};
constexpr TextColor yellow{TextColor::yellow};
constexpr TextColor blue{TextColor::blue};
constexpr TextColor magenta{TextColor::magenta};
constexpr TextColor cyan{TextColor::cyan};
constexpr TextColor white{TextColor::white};
constexpr TextColor defaultText{TextColor::defaultCol};

constexpr BgColor onBlack{BgColor::black};
constexpr BgColor onRed{BgColor::red};
constexpr BgColor onGreen{BgColor::green};
constexpr BgColor onYellow{BgColor::yellow};
constexpr BgColor onBlue{BgColor::blue};
constexpr BgColor onMagenta{BgColor::magenta};
constexpr BgColor onCyan{BgColor::cyan};
constexpr BgColor onWhite{BgColor::white};
constexpr BgColor onDefault{BgColor::defaultCol};


template <terminalColorTarget target>
class SPIRIT_API RgbTerminalColor
{
public:
    constexpr RgbTerminalColor(sp::Uint8 r, sp::Uint8 g, sp::Uint8 b)
        : _r{r}, _g{g}, _b{b}
    {
    }


    template <terminalColorTarget t>
    friend std::ostream &
    operator<<(std::ostream & os, RgbTerminalColor<t> c);


    [[nodiscard]] sp::Uint8
    r() const;


    [[nodiscard]] sp::Uint8
    g() const;


    [[nodiscard]] sp::Uint8
    b() const;


private:
    static constexpr sp::Int32 declareRgbColor    = 8;
    static constexpr sp::Int32 declareRgbSequence = 2;
    static constexpr terminalColorTarget t{target};

    sp::Uint8 _r;
    sp::Uint8 _g;
    sp::Uint8 _b;
};


typedef RgbTerminalColor<terminalColorTarget::text> RgbTextColor;
typedef RgbTerminalColor<terminalColorTarget::background> RgbBgColor;


// TODO: Add the nots, make a combined modificators reset
class SPIRIT_API TerminalModifier
{
public:
    /// I recommend only those marked with a '(*)',
    /// as they are more likely to be supported
    enum modifier : sp::Int32
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


    constexpr TerminalModifier(modifier mod) : mod{mod} {}


    friend std::ostream &
    operator<<(std::ostream & os, TerminalModifier mod);


private:
    modifier mod;
};


constexpr TerminalModifier reset{TerminalModifier::reset};
constexpr TerminalModifier bold{TerminalModifier::bold};
constexpr TerminalModifier faint{TerminalModifier::faint};
constexpr TerminalModifier italic{TerminalModifier::italic};
constexpr TerminalModifier underline{TerminalModifier::underline};
constexpr TerminalModifier slowBlink{TerminalModifier::slowBlink};
constexpr TerminalModifier fastBlink{TerminalModifier::fastBlink};
constexpr TerminalModifier swapColors{TerminalModifier::swapColors};
constexpr TerminalModifier conceal{TerminalModifier::conceal};
constexpr TerminalModifier crossed{TerminalModifier::crossed};


SPIRIT_API std::string
colorOutput(const std::string & txt,
            TextColor txtCol,
            BgColor bgCol,
            bool resetColorsAfter = true);


SPIRIT_API std::string
colorText(const std::string & txt, TextColor txtCol, bool resetColorsAfter = true);


SPIRIT_API std::string
colorBg(const std::string & txt, BgColor bgCol, bool resetColorsAfter = true);


template <terminalColorTarget t>
SPIRIT_API std::string
textGradient(const std::string & txt,
             RgbTerminalColor<t> firstChar,
             RgbTerminalColor<t> lastChar,
             bool resetColorAfter = true);


SPIRIT_API std::string
modify(const std::string & txt, TerminalModifier mod, bool resetAfter = true);


SPIRIT_API std::string
modify(const std::string & txt,
       std::initializer_list<TerminalModifier> mods,
       bool resetAfter = true);


} // namespace IO


#define SPIRIT_APPLY_LEVEL(PRINTABLE, TXT_COLOR, BACKGROUND_COLOR)             \
    sp::IO::colorOutput(sp::IO::format(PRINTABLE), TXT_COLOR, BACKGROUND_COLOR)

// Requires at least one parameter
#define SPIRIT_TRACE(...)                                                      \
    SPDLOG_TRACE(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),               \
                                    sp::IO::defaultText,                       \
                                    sp::IO::onDefault))


#define SPIRIT_INFO(...)                                                       \
    SPDLOG_INFO(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),                \
                                   sp::IO::green,                              \
                                   sp::IO::onDefault))


#define SPIRIT_WARN(...)                                                       \
    SPDLOG_WARN(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),                \
                                   sp::IO::yellow,                             \
                                   sp::IO::onDefault))


#define SPIRIT_ERROR(...)                                                      \
    SPDLOG_ERROR(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),               \
                                    sp::IO::red,                               \
                                    sp::IO::onDefault))


#define SPIRIT_CRITICAL(...)                                                   \
    SPDLOG_CRITICAL(SPIRIT_APPLY_LEVEL(sp::IO::format(__VA_ARGS__),            \
                                       sp::IO::black,                          \
                                       sp::IO::onRed))


namespace priv
{
// TODO: Users should not be able to call those, only access to spiritInit


// https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters
class SPIRIT_API AnsiSGR
{

    // When we cannot enable terminal colors, this will be set to false.
    // functions applying color to text will then have no effect and return an untouched string
    static bool enableAnsiColor;


    friend std::shared_ptr<spdlog::logger>
    initLogging(std::ostream &); // disables colors if unsupported

public:
    static constexpr const char * CSI = "\u001b[";
    static constexpr const char * end = "m";

    // codes are actually in the [ 0x20 , 0x7E ] range,
    // we use int for proper output
    static std::ostream &
    putCode(std::ostream & os, sp::Int32 code);


    template <sp::Int32 nCodes>
    static std::ostream &
    putCodes(std::ostream & os, sp::Int32 (&codes)[nCodes]);


    static std::ostream &
    reset(std::ostream & os);
};


bool
enableWindowsColors();


std::shared_ptr<spdlog::logger>
initLogging(std::ostream & os = std::cout);

} // namespace priv


} // namespace sp


#include "Logging_inl.hpp"


#endif // SPIRIT_ENGINE_LOGGING_HPP
