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

#ifndef SPIRIT_ENGINE_TIMER_H
#define SPIRIT_ENGINE_TIMER_H


#include <chrono>

#include "SPIRIT/Core.hpp"


namespace sp
{


//////////////////////////////////////////////////////////
/// \brief High resolution Timer for simple Benchmarking
///
//////////////////////////////////////////////////////////
class Timer
{
public:

    //////////////////////////////////////////////////////////
    /// \brief Start a timer (stopwatch)
    //////////////////////////////////////////////////////////
    Timer(bool startNow = true);


    void
    pause()
    {
        if ( !paused )
        {
            elapsed += std::chrono::high_resolution_clock::now() - time;
            paused = true;
        }
    }


    void
    start()
    {
        if ( paused )
        {
            time = std::chrono::high_resolution_clock::now();
            paused = false;
        }
    }


    bool
    isPaused() const
    { return paused; }


    //////////////////////////////////////////////////////////
    /// \brief Reset the time
    //////////////////////////////////////////////////////////
    void
    reset();


    std::chrono::nanoseconds
    getElapsed() const;


    //////////////////////////////////////////////////////////
    /// \brief Get the nanoseconds since last reset (or creation)
    ///
    /// \return Nanoseconds elapsed
    //////////////////////////////////////////////////////////
    double
    getElapsedNano() const;


    //////////////////////////////////////////////////////////
    /// \brief Get the microseconds since last reset (or creation)
    ///
    /// \return Microseconds elapsed
    //////////////////////////////////////////////////////////
    double
    getElapsedMicro() const;


    //////////////////////////////////////////////////////////
    /// \brief Get the milliseconds since last reset (or creation)
    ///
    /// \return Milliseconds elapsed
    //////////////////////////////////////////////////////////
    double
    getElapsedMilli() const;


    //////////////////////////////////////////////////////////
    /// \brief Get the seconds since last reset (or creation)
    ///
    /// \return Seconds elapsed
    //////////////////////////////////////////////////////////
    double
    getElapsedSec() const;


protected:
    std::chrono::time_point< std::chrono::high_resolution_clock,
            std::chrono::nanoseconds > time;

    std::chrono::nanoseconds elapsed{0};
    bool paused = true;
};

} // namespace sp



#endif //SPIRIT_ENGINE_TIMER_H
