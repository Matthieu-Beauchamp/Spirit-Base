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


#ifndef SPIRIT_BASE_HPP
#define SPIRIT_BASE_HPP


////////////////////////////////////////////////////////////
/// \defgroup Base Base
///
/// \brief Base module of the Spirit library
/// 
/// The Base module contains the basic faciliities on which 
/// Spirit relies across all modules.
///
/// For users, the Configuration module is the most relevant.
/// 
/// Users may also wish to use the Ansi aware Logging facilities
/// for colored output (and more).
/// 
////////////////////////////////////////////////////////////


#include "config.hpp"

#include "Logging/AnsiEscape.hpp"
#include "Logging/AnsiStream.hpp"

// Pretty function output for Loggers
// TODO: We should fork spdlog and modify the tweakme.hpp
#ifndef SPDLOG_FUNCTION
#ifdef __PRETTY_FUNCTION__
#    define SPDLOG_FUNCTION __PRETTY_FUNCTION__
#else
#    define SPDLOG_FUNCTION __FUNCTION__
#endif
#endif
#include "Logging/Format.hpp"
#include "Logging/Message.hpp"
#include "Logging/Logger.hpp"

// include utils?

#include "Error/Error.hpp"


#endif // SPIRIT_BASE_HPP
