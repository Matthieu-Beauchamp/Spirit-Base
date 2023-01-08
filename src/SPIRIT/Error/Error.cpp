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


#include "SPIRIT/Base/Error/Error.hpp"

// #include <stacktrace> // in C++23
#include "boost/stacktrace.hpp"


namespace sp
{

typedef boost::stacktrace::stacktrace Stacktrace;
typedef boost::stacktrace::frame Frame;

std::string
stringifyStacktrace(const Stacktrace & st)
{
    ///////////////////////////////////////////////////////
    // Some implementations will always return empty strings here...
    // so making our own sucks
    // for (const auto & bt : st)
    // {
    //     std::cout << bt.name();
    //     std::cout << bt.source_file();
    //     std::cout << bt.source_line();
    // }

    return boost::stacktrace::to_string(st);
}

SpiritError::SpiritError()
    : explanation{
        std::string{"Stacktrace:\n"} + stringifyStacktrace(Stacktrace{}) + "\n"}
{
}


} // namespace sp
