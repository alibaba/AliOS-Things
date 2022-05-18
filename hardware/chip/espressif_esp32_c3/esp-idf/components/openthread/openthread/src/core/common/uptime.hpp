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
 *   This file includes definitions for tracking device's uptime.
 */

#ifndef UPTIME_HPP_
#define UPTIME_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_UPTIME_ENABLE

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/string.hpp"
#include "common/time.hpp"
#include "common/timer.hpp"

namespace ot {

/**
 * This class implements tracking of device uptime (in msec).
 *
 */
class Uptime : public InstanceLocator, private NonCopyable
{
public:
    /**
     * This constructor initializes an `Uptime` instance.
     *
     * @param[in] aInstance   The OpenThread instance.
     *
     */
    explicit Uptime(Instance &aInstance);

    /**
     * This method returns the current device uptime (in msec).
     *
     * The uptime is maintained as number of milliseconds since OpenThread instance was initialized.
     *
     * @returns The uptime (number of milliseconds).
     *
     */
    uint64_t GetUptime(void) const;

    /**
     * This method gets the current uptime as a human-readable string.
     *
     * The string follows the format "<hh>:<mm>:<ss>.<mmmm>" for hours, minutes, seconds and millisecond (if uptime is
     * shorter than one day) or "<dd>d.<hh>:<mm>:<ss>.<mmmm>" (if longer than a day).
     *
     * If the resulting string does not fit in @p aBuffer (within its @p aSize characters), the string will be
     * truncated but the outputted string is always null-terminated.
     *
     * @param[out] aBuffer   A pointer to a char array to output the string.
     * @param[in]  aSize     The size of @p aBuffer (in bytes). Recommended to use `OT_UPTIME_STRING_SIZE`.
     *
     */
    void GetUptime(char *aBuffer, uint16_t aSize) const;

    /**
     * This method converts an uptime value (number of milliseconds) to a human-readable string.
     *
     * The string follows the format "<hh>:<mm>:<ss>.<mmmm>" for hours, minutes, seconds and millisecond (if uptime is
     * shorter than one day) or "<dd>d.<hh>:<mm>:<ss>.<mmmm>" (if longer than a day).
     *
     * @param[in]    aUptime  The uptime to convert.
     * @param[inout] aWriter  A `StringWriter` to append the converted string to.
     *
     */
    static void UptimeToString(uint64_t aUptime, StringWriter &aWriter);

private:
    static constexpr uint32_t kTimerInterval   = (1 << 30);
    static constexpr uint32_t kOneSecondInMsec = 1000;
    static constexpr uint32_t kOneMinuteInMsec = 60 * kOneSecondInMsec;
    static constexpr uint32_t kOneHourInMsec   = 60 * kOneMinuteInMsec;
    static constexpr uint32_t kOneDayInMsec    = 24 * kOneHourInMsec;

    static_assert(static_cast<uint32_t>(4 * kTimerInterval) == 0, "kTimerInterval is not correct");

    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);

    TimeMilli  mStartTime;
    uint32_t   mOverflowCount;
    TimerMilli mTimer;
};

} // namespace ot

#endif // OPENTHREAD_CONFIG_UPTIME_ENABLE

#endif // UPTIME_HPP_
