////////////////////////////////////////////////////////////
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


#ifndef SPIRIT_ENGINE_LOGGING_INL
#define SPIRIT_ENGINE_LOGGING_INL

#include <sstream>
#include <string>

#include "SPIRIT/Logging/Logging.hpp"


namespace sp
{
namespace IO
{


// ////////////////////////////////////////////////////////
template <class... Args>
std::string
format(const std::string & str, Args &&... args)
{
    return fmt::format(fmt::runtime(str), std::forward<Args>(args)...);
}

// ////////////////////////////////////////////////////////
template <class T, std::enable_if_t<sp::traits::Printable<T>::value, bool>>
std::string
format(const T & printable)
{
    // Fixes a bug (Mingw64) where when we call format with a single argument,
    //      fmt::format complains that there is no known conversion
    //      to format_string
    std::stringstream txt{};
    txt << printable;

    return txt.str();
}

} // namespace IO
} // namespace sp


#endif // SPIRIT_ENGINE_LOGGING_INL
