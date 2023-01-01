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

// Why is this not available with std::void_t in gcc with c++20?
template <typename... Ts>
struct make_void
{
    typedef void type;
};

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;


////////////////////////////////////////////////////////////
/// \ingroup Concepts
/// \brief Any object which can be sent to an output stream with operator <<
///
/// That is, defines Printable<T>::value as true
///     for any object for which \code std::cout << object; \endcode is valid.
/// Otherwise, defines Printable<T>::value as false
///
////////////////////////////////////////////////////////////
template <class T, class = void>
struct Printable : public std::false_type
{
};


template <class T>
struct Printable<
    T,
    void_t<decltype(std::declval<std::ostream &>() << std::declval<T>())>>
    : public std::true_type
{
};


////////////////////////////////////////////////////////////
/// \ingroup Concepts
/// \brief Any object that can be used to do IO (<< or >> with const char *)
///
////////////////////////////////////////////////////////////
template <class T, class = void>
struct isStream : public std::false_type
{
};

// out stream
template <class T>
struct isStream<T, void_t<decltype(std::declval<T &>() << std::declval<const char *>())>>
    : public std::true_type
{
};

// in stream
template <class T>
struct isStream<T, void_t<decltype(std::declval<T &>() >> std::declval<const char *>())>>
    : public std::true_type
{
};


////////////////////////////////////////////////////////////
// Base class of Ts detection
////////////////////////////////////////////////////////////

template <class BaseType, class... Ts>
struct areOfBaseType : public std::integral_constant<
                           bool,
                           std::conjunction<std::is_base_of<
                               std::remove_cvref_t<BaseType>,
                               std::remove_cvref_t<Ts>>...>::value>
{
};


namespace details
{

// Details for selecting the deepest base of a set of Ts,
// given a list of bases.

template <class... Bs>
struct Bases
{
};

template <>
struct Bases<>
{
    // first should trigger compile failure (none available)
    using others = Bases<>;

    template <class... B2s>
    using extend = Bases<B2s...>;

    template <class B2>
    using prepend = Bases<B2>;
};

template <class B>
struct Bases<B>
{
    typedef B first;
    using others = Bases<>;

    template <class... B2s>
    using extend = Bases<B, B2s...>;

    template <class B2>
    using prepend = Bases<B2, B>;


    template <class... Ts>
    struct BasesOf
    {
        typedef std::
            conditional_t<sp::traits::areOfBaseType<B, Ts...>::value, Bases<B>, others>
                Bases_t;
    };

    struct Deepest
    {
        typedef Bases<B> Kept;
        typedef B Deepest_t;
    };

    template <class... Ts>
    struct DeepestBaseOf
    {
        typedef typename BasesOf<Ts...>::Bases_t BasesChain;
        typedef typename BasesChain::Deepest::Deepest_t DeepestBaseOf_t;
    };
};

template <class B, class... Bs>
struct Bases<B, Bs...>
{
    typedef B first;
    using others = Bases<Bs...>;

    template <class... B2s>
    using extend = Bases<B, Bs..., B2s...>;

    // necessary to keep ordering (mostly for tests)
    template <class B2>
    using prepend = Bases<B2, B, Bs...>;


    template <class... Ts>
    struct BasesOf
    {
        typedef typename Bases<Bs...>::BasesOf<Ts...>::Bases_t SkipThisOne;
        typedef typename SkipThisOne::prepend<B> Continue;

        typedef std::conditional_t<sp::traits::areOfBaseType<B, Ts...>::value, Continue, SkipThisOne>
            Bases_t;
    };

    struct Deepest
    {
        typedef std::conditional_t<
            sp::traits::areOfBaseType<B, Bs...>::value,
            Bases<Bs...>,
            Bases<Bs..., B>>
            Kept;

        typedef typename Kept::Deepest::Deepest_t Deepest_t;
    };

    template <class... Ts>
    struct DeepestBaseOf
    {
        typedef typename BasesOf<Ts...>::Bases_t BasesChain;
        typedef typename BasesChain::Deepest::Deepest_t DeepestBaseOf_t;
    };
};

} // namespace details

////////////////////////////////////////////////////////////
/// \brief Finds the most derived base common to all Ts, given a set of Bases Bs.
/// 
/// This was needed to correcty detect the type of sp::Escapes<...>
/// such that it will inherit from the common denominator (AnsiEscape, TextStyle, ...)
/// 
/// Diamond inheritance is not supported.
/// Furthemore, Bases are expected to be a part of the same tree.
/// 
/// given the inheritance tree:
/// \code
///     A
///   B  C
/// D
/// \endcode
///
/// then:
/// \code
/// Bases<A, B, C, D>::DeepestOf<A, B, C, D> -> A
/// Bases<A, B, C, D>::DeepestOf<B, D> -> B
/// \endcode
/// 
////////////////////////////////////////////////////////////
template <class... Bs>
struct Bases
{
    template <class... Ts>
    using DeepestOf = typename details::Bases<Bs...>::DeepestBaseOf<Ts...>::DeepestBaseOf_t;
};



} // namespace traits

} // namespace sp


#endif // SPIRIT_ENGINE_TRAITS_HPP
