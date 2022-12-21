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


#ifndef SPIRIT_ENGINE_TRAITS_HPP
#define SPIRIT_ENGINE_TRAITS_HPP

#include <iosfwd>
#include <type_traits>

namespace sp
{

////////////////////////////////////////////////////////////
/// \defgroup Concepts Concepts
///
/// \brief Defines helpers for identifying specific traits of objetcs and functions
///
////////////////////////////////////////////////////////////
namespace traits
{

// TODO: Macro magic here?
// template<class T, class = void>
// struct ValidExpression : std::false_type
// {
// };

// template<class T, class U>
// struct ValidExpression<T> : std::false_type
// {
// };


////////////////////////////////////////////////////////////
/// \ingroup Concepts
/// \brief Any object which can be sent to an output stream with operator <<
///
/// That is, defines Printable<T>::value as true
///     for any object for which \code std::cout << object; \endcode is valid.
/// Otherwise, defines Printable<T>::value as false
///
////////////////////////////////////////////////////////////

// Probably does not work
template <class T, class = void>
struct Printable : public std::false_type
{
};


template <class T>
struct Printable<
    T,
    std::enable_if_t<true, decltype(std::declval<std::ostream &>() << std::declval<T>())>>
    : public std::true_type
{
};


// Does not work
template <class T, class = void>
struct isStream : public std::false_type
{
};

template <class T>
struct isStream<
    T,
    std::enable_if_t<std::is_base_of<std::basic_ios<typename T::char_type>, T>::value, bool>>
    : public std::true_type
{
};



} // namespace traits

} // namespace sp


#endif // SPIRIT_ENGINE_TRAITS_HPP
