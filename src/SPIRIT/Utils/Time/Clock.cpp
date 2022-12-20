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



#include <thread>

#include "SPIRIT/Utils/Time/Clock.hpp"


namespace sp
{

Clock::Nanoseconds
Clock::getCurrentDt() const
{
    return this->timer.getElapsed();
}


Clock::Clock(Nanoseconds minimumTickPeriod)
{
    this->setMinimumTickPeriod(minimumTickPeriod);
}


sp::Clock::Nanoseconds
sp::Clock::tick()
{
    Nanoseconds dt = this->timer.getElapsed();

    std::this_thread::sleep_for(tickPeriod - dt);
    dt = this->timer.getElapsed();
    this->timer.reset();

    this->ticks.push_back(dt);
    if ( this->ticks.size() > 10 )
        this->ticks.pop_front();

    return dt;
}


Clock::Nanoseconds
Clock::getAverageTick() const
{
    Nanoseconds allTicks{0};
    for ( const auto & tick : this->ticks )
        allTicks += tick;

    auto nTicks = this->ticks.size();
    nTicks = nTicks == 0 ? 1 : nTicks;

    return allTicks / nTicks;
}


float
Clock::getTicksPerSecond() const
{
    float avgTick = ((float) this->getAverageTick().count() / 1e9f);
    return avgTick == 0.f ? 0.f : 1.f / avgTick;
}


void
Clock::setMinimumTickPeriod(Clock::Nanoseconds minTime)
{
    this->tickPeriod = minTime;
}


} // namespace sp
