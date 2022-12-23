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


#include "Timer.hpp"

#include <queue>


#ifndef SPIRIT_ENGINE_CLOCK_H
#    define SPIRIT_ENGINE_CLOCK_H


namespace sp
{

class Clock
{
public:

    typedef std::chrono::nanoseconds Nanoseconds;

private:

    Timer timer{};

    Nanoseconds tickPeriod{0}; // don't wait on ticks

    // TODO: use a circular buffer instead
    std::deque<Nanoseconds> ticks{};

public:

    Clock(Nanoseconds minimumTickPeriod = Nanoseconds{0});

    Nanoseconds
    getCurrentDt() const;

    Nanoseconds
    tick();

    Nanoseconds
    getAverageTick() const;

    float
    getTicksPerSecond() const;

    void
    setMinimumTickPeriod(Nanoseconds minTime);
    
    Nanoseconds
    getMinimumTickPeriod() const
    {
        return this->tickPeriod;
    }
};


struct WindowClock : private sp::Clock
{
public:

    using Clock::Clock;
    using Clock::getCurrentDt;
    using Clock::tick;

    std::chrono::nanoseconds
    getFrameTime() const
    {
        return this->getAverageTick();
    }


    float
    getFps() const
    {
        return this->getTicksPerSecond();
    }


    void
    setFps(float fps = 0)
    {
        this->setMinimumTickPeriod(std::chrono::nanoseconds{
            (sp::Int32)(fps == 0 ? 0 : 1e9 / fps)});
    }


    void
    setFrameTime(std::chrono::nanoseconds dt = std::chrono::nanoseconds{0})
    {
        this->setMinimumTickPeriod(dt);
    }
};


} // namespace sp


#endif // SPIRIT_ENGINE_CLOCK_H
