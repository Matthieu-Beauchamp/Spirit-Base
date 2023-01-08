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


#ifndef SPIRIT_LOGGING_HPP
#define SPIRIT_LOGGING_HPP

////////////////////////////////////////////////////////////
/// \ingroup Base
/// \defgroup Logging Logging
/// \brief Logging facilities for Spirit modules (and users)
///
/// Wraps around spdlog's library, notably providing ansi escapes
/// aware streams and sinks. This allows for output to be colored only
/// when the receiving FILE is a color terminal.
/// Streamable Message objects are also provided. They contain source location
/// information and can be streamed to loggers. This avoids the need for
/// logging macros.
///
/// for more information about spdlog, you may wish to refer to
/// https://github.com/gabime/spdlog/wiki
///
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
/// \ingroup Base
/// \defgroup Logging Logging
/// \brief Ansi aware logging facilities
/// 
/// Spirit modules will use these logging facilities to inform 
/// the user of any relevant informations. Multiple log levels
/// allows filtering what information the user wants to see (see Configuration).
/// 
/// The user may wish to use these classes for their own logging.
/// The user may also wish to modify the behavior of Spirit's logger
/// (see Logger).
/// 
////////////////////////////////////////////////////////////

#include "AnsiEscape.hpp"
#include "AnsiStream.hpp"

#include "Format.hpp"
#include "Logger.hpp"

#endif // SPIRIT_LOGGING_HPP
