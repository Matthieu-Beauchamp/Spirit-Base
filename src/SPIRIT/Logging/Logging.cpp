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

#include <iostream>

#include "SPIRIT/Logging/Logging.hpp"


#if defined(SPIRIT_OS_WINDOWS)
#    include <sstream>
#    include <windows.h>
#endif

namespace sp
{

namespace IO
{

// ////////////////////////////////////////////////////////
void setLogsOutput(std::ostream & os)
{
}


// ////////////////////////////////////////////////////////
std::ostream &
operator<<(std::ostream & os, TerminalModifier mod)
{
    return priv::AnsiSGR::putCode(os, mod.mod);
}

// ////////////////////////////////////////////////////////
std::string
colorOutput(const std::string & txt,
            TextColor txtCol,
            BgColor bgCol,
            bool resetColorsAfter)
{
    std::stringstream ss;
    ss << txtCol << bgCol << txt;
    if (resetColorsAfter)
        ss << defaultText << onDefault;
    return ss.str();
}

// ////////////////////////////////////////////////////////
std::string
colorText(const std::string & txt, TextColor txtCol, bool resetColorsAfter)
{
    return colorOutput(txt, txtCol, onDefault, resetColorsAfter);
}

// ////////////////////////////////////////////////////////
std::string
colorBg(const std::string & txt, BgColor bgCol, bool resetColorsAfter)
{
    return colorOutput(txt, defaultText, bgCol, resetColorsAfter);
}


// ////////////////////////////////////////////////////////
std::string
modify(const std::string & txt, TerminalModifier mod, bool resetAfter)
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
       std::initializer_list<TerminalModifier> mods,
       bool resetAfter)
{
    std::stringstream ss{};

    for (TerminalModifier mod : mods)
        ss << mod;

    ss << txt;
    if (resetAfter)
        ss << reset;

    return ss.str();
}

} // namespace IO

namespace priv
{

bool AnsiSGR::enableAnsiColor = true;

// ////////////////////////////////////////////////////////
std::ostream &
AnsiSGR::putCode(std::ostream & os, sp::Int32 code)
{
    if (enableAnsiColor)
        os << CSI << code << end;
    return os;
}


// ////////////////////////////////////////////////////////
std::ostream &
AnsiSGR::reset(std::ostream & os)
{
    return putCode(os, 0);
}

// ////////////////////////////////////////////////////////
bool
enableWindowsColors()
{
#if defined(SPIRIT_OS_WINDOWS)
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // for std::cout

    if (hOut != INVALID_HANDLE_VALUE)
    {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode))
        {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(hOut, dwMode))
            {
                std::stringstream what;
                what << "Error code: " << GetLastError();

                std::cout << what.str()
                          << "\n"
                             "Error occured while trying to enable "
                             "terminal virtual processing for Windows.\n"
                             "\tDisabled color output";
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

#endif
    return true;
}


// //////////////////////////////////////////////////////
std::shared_ptr<spdlog::logger>
initLogging(std::ostream & os)
{
    if (!priv::enableWindowsColors())
    {
        priv::AnsiSGR::enableAnsiColor = false;
    }

    std::string spiritName = IO::textGradient("Spirit",
                                              IO::RgbTextColor{0, 255, 255},
                                              IO::RgbTextColor{255, 100, 255},
                                              false);
    ;
#if defined(SPIRIT_OS_WINDOWS)
    spiritName = sp::IO::modify(spiritName, sp::IO::bold);
#else
    spiritName = sp::IO::modify(spiritName, {sp::IO::bold, sp::IO::italic});
#endif

    auto spiritLogger = spdlog::stdout_color_mt(spiritName);
    

    set_default_logger(spiritLogger);
    spiritLogger->set_level(spdlog::level::trace);

    std::string pattern
        = "[%H:%M:%S.%e][%n][" + sp::IO::modify("%^%l%$", sp::IO::bold) + "]"
          + " from "
          + sp::IO::modify(sp::IO::colorText("%s:%#", sp::IO::cyan),
                           sp::IO::underline)
          + " -> " + sp::IO::colorText("%!", sp::IO::magenta)
          + " :\n"
            "> %v\n"
            "------------------------------------------------------------------"
            "--------------";

    spiritLogger->set_pattern(pattern);
    spdlog::set_pattern(pattern); // globally


    if (!priv::AnsiSGR::enableAnsiColor)
        SPIRIT_WARN("Could not enable terminal colors");


    return spiritLogger;
}


} // namespace priv

} // namespace sp
