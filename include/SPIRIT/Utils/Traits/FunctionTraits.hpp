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


#include <functional>
#include <cstdio>

#ifndef SPIRIT_ENGINE_FUNCTIONTRAITS_HPP
#define SPIRIT_ENGINE_FUNCTIONTRAITS_HPP


namespace sp
{


// https://stackoverflow.com/a/9065203

template< class Signature >
struct function
{
    typedef Signature signature;
    typedef Signature function_type;
};


template< class Signature >
struct function_traits : public function< Signature >
{
};


template< class Return, class ...Args >
struct function_traits< std::function< Return(Args...)>>
        : public function< std::function< Return(Args...)>>
{
    typedef Return result_type;

    static constexpr size_t arg_count = sizeof...(Args);

    template< size_t i >
    struct Arg
    {
        typedef typename std::tuple_element< i, std::tuple< Args...>>::type type;
    };

};


template< class Return, class ...Args >
struct function_traits< Return(Args...) >
        : public function< Return(Args...) >
{
    typedef Return result_type;

    static constexpr size_t arg_count = sizeof...(Args);

    template< size_t i >
    struct Arg
    {
        typedef typename std::tuple_element< i, std::tuple< Args...>>::type type;
    };

};

template< class Return, class ...Args >
struct function_traits< Return(*)(Args...) >
        : public function< Return(*)(Args...) >
{
    typedef Return result_type;

    static constexpr size_t arg_count = sizeof...(Args);

    template< size_t i >
    struct Arg
    {
        typedef typename std::tuple_element< i, std::tuple< Args...>>::type type;
    };
};


} // namespace sp


#endif //SPIRIT_ENGINE_FUNCTIONTRAITS_HPP
