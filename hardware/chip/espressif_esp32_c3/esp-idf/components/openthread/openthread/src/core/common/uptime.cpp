/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements mechanism to track device's uptime.
 */

#include "uptime.hpp"

#if OPENTHREAD_CONFIG_UPTIME_ENABLE

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "common/instance.hpp"
#include "common/locator_getters.hpp"

namespace ot {

Uptime::Uptime(Instance &aInstance)
    : InstanceLocator(aInstance)
    , mStartTime(TimerMilli::GetNow())
    , mOverflowCount(0)
    , mTimer(aInstance, HandleTimer)
{
    mTimer.FireAt(mStartTime + kTimerInterval);
}

uint64_t Uptime::GetUptime(void) const
{
    TimeMilli now           = TimerMilli::GetNow();
    uint32_t  overflowCount = mOverflowCount;

    // `mTimer` is scheduled with duration `kTimerInterval = (1 << 30)`
    // so it takes four timer fires for us to go over the entire 32-bit
    // range and get back to `mStartTime` (at which point we increment
    // the `mOverflowCount` in `HandleTimer()`).
    //
    // Here, we handle the corner case where we may have reached or
    // passed beyond the `mStartTime` but the `mTimer` is not yet
    // handled. In this case we increment `overflowCount` (which is
    // our copy of the current `mOverflowCount` value). We use the
    // check `(mTimer.GetFireTime() == mStartTime)` to determine if
    // we are in the last of the four timer fires before overflow.

    if ((mTimer.GetFireTime() == mStartTime) && (now >= mStartTime))
    {
        overflowCount++;
    }

    // The uptime is returned as a `uint64_t`. The `overflowCount`
    // gives the higher 32 bits, and `(now - mStartTime)` gives the
    // lower 32 bits (which is always correct even under the corner
    // case where the `HandleTimer()` is not yet handled).

    return (static_cast<uint64_t>(overflowCount) << 32) + (now - mStartTime);
}

void Uptime::GetUptime(char *aBuffer, uint16_t aSize) const
{
    StringWriter writer(aBuffer, aSize);

    UptimeToString(GetUptime(), writer);
}

void Uptime::HandleTimer(Timer &aTimer)
{
    aTimer.Get<Uptime>().HandleTimer();
}

void Uptime::HandleTimer(void)
{
    if (mTimer.GetFireTime() == mStartTime)
    {
        mOverflowCount++;
    }

    mTimer.FireAt(mTimer.GetFireTime() + kTimerInterval);
}

static uint32_t DivideAndGetRemainder(uint32_t &aDividend, uint32_t aDivisor)
{
    // Returns the quotient of division `aDividend / aDivisor` and updates
    // `aDividend` to returns the remainder

    uint32_t quotient = aDividend / aDivisor;

    aDividend -= quotient * aDivisor;

    return quotient;
}

void Uptime::UptimeToString(uint64_t aUptime, StringWriter &aWriter)
{
    uint64_t days = aUptime / kOneDayInMsec;
    uint32_t remainder;
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;

    if (days > 0)
    {
        aWriter.Append("%lud.", days);
        aUptime -= days * kOneDayInMsec;
    }

    remainder = static_cast<uint32_t>(aUptime);
    hours     = DivideAndGetRemainder(remainder, kOneHourInMsec);
    minutes   = DivideAndGetRemainder(remainder, kOneMinuteInMsec);
    seconds   = DivideAndGetRemainder(remainder, kOneSecondInMsec);

    aWriter.Append("%02u:%02u:%02u.%03u", hours, minutes, seconds, remainder);
}

} // namespace ot

#endif // OPENTHREAD_CONFIG_UPTIME_ENABLE
