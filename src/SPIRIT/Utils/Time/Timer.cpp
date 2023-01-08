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


#include "SPIRIT/Base/Utils/Time/Timer.hpp"


namespace sp
{


Timer::Timer(bool startNow )
{
    if ( startNow )
        start();
}


void
Timer::reset()
{
    time = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::nanoseconds{0};
}


std::chrono::nanoseconds
Timer::getElapsed() const
{
    if ( paused )
        return elapsed;

    return elapsed + std::chrono::high_resolution_clock::now() - time;
}


double
Timer::getElapsedNano() const
{
    return getElapsed().count();
}


double
Timer::getElapsedMicro() const
{
    return this->getElapsedNano() / 1e3;
}


double
Timer::getElapsedMilli() const
{
    return this->getElapsedNano() / 1e6;
}


double
Timer::getElapsedSec() const
{
    return this->getElapsedNano() / 1e9;
}


} // namespace sp
