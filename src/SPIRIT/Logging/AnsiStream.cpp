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


#include "SPIRIT/Logging/AnsiStream.hpp"

#include "spdlog/details/os.h"

#include <stdio.h>

// TODO: Include windows...

namespace sp
{

sp::AnsiStream ansiOut{stdout};
sp::AnsiStream ansiErr{stderr};


// TODO: Does not check for given file!
// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences#samples
// incomplete, we if ostream is std::cerr or std::clog, theyre not the same as stdout
//
// TODO: SEE: https://github.com/agauniyal/rang
//  better checking (ie msys, pipes, ...) and the native windows API usage
bool
enableVirtualTerminal(FILE * file)
{
#if defined(SPIRIT_OS_WINDOWS)
    // Set output mode to handle virtual terminal sequences
    // TODO: Once on windows, change this to use _fileno(file)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
        // return GetLastError();
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
        // return GetLastError();
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
        // return GetLastError();
    }

    return true;
#endif
    return true;
}


// For Windows https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

// some info and referenes https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

// https://github.com/agauniyal/rang is the original reference,
// His terminal checkin is better than spdlog's, we should use it instead.

// TODO: spdlog does not check for Windows virtual terminal emulation.
bool
supportsAnsi(FILE * file)
{
    return spdlog::details::os::in_terminal(file)
           && spdlog::details::os::is_color_terminal()
           && enableVirtualTerminal(file);
}

} // namespace sp
