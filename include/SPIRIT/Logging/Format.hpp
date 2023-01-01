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


#ifndef SPIRIT_FORMAT_HPP
#define SPIRIT_FORMAT_HPP

#include "SPIRIT/Concepts/Concepts.hpp"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/ostr.h"

#include <string>
#include <string_view>

namespace sp
{


////////////////////////////////////////////////////////////
/// \brief Format any number of printable arguments into a string
///
/// Works for standalone objects with have an operator<< overload
/// with std::ostream and complex format strings with any number of
/// format arguments:
/// \code sp::format("{} != {}", 1, 2); \endcode
///
/// See fmtlib's reference for format strings syntax:
/// https://fmt.dev/latest/syntax.html.
///
/// For objects with an overloaded operator<<, 
/// This file must be included for the format function to work.
////////////////////////////////////////////////////////////
template <
    class... Args,
    std::enable_if_t<std::conjunction<sp::traits::Printable<Args>...>::value, bool> = true>
std::string
format(std::string_view str, Args &&... args)
{
    // Tried to make it so we could detect if the strings passed to constructors
    // are constexpr (to use fmt::format_string_t), could not make it work while
    // still allowing dynamic strings. We will settle on string_view
    return fmt::vformat(str, fmt::make_format_args(args...));
}

template <
    class T,
    std::enable_if_t<
        sp::traits::Printable<T>::value
            && !std::is_convertible<T, std::string_view>::value,
        bool> = true>
std::string
format(const T & printable)
{
    return sp::format("{}", printable);
}


} // namespace sp


#endif // SPIRIT_FORMAT_HPP
