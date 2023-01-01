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

#include "SPIRIT/Logging/Logger.hpp"

namespace sp
{

std::string spiritPattern(){
        std::string basePattern
            = "[%T.%e][%n][%^%l%$]"; // [H:M:S.ms][name][log level (colored)]
        
        std::string sourceLoc = sp::format(
            " {}%s:%#{} -> {}{}%!{}\n", // file:line -> func
            sp::cyan,
            sp::reset,
            sp::magenta,
            sp::bold,
            sp::reset
        );

        std::string msg = "> %v\n";
        for (int _ = 0; _ < 60; ++_) { msg += "-"; }
        msg += "\n";

        // [H:M:S.ms][name][loglevel (colored)] file:line -> func
        // > msg...
        // ------------------------------------------------------------
        return basePattern + sourceLoc + msg;
}

sp::LoggerPtr
spiritLogger()
{
    static sp::LoggerPtr logger{nullptr};

    if (!logger)
    {
        constexpr sp::RgbFgColor lightBlue{66, 230, 245};
        constexpr sp::RgbFgColor pink{245, 66, 212};
        std::string name = sp::format("{}{}", sp::bold, sp::FgGradient("Spirit", lightBlue, pink));

        logger = makeLogger<sp::FileSink_mt>(name, stdout);

        logger->set_pattern(spiritPattern());
    }

    return logger;
}


} // namespace sp
