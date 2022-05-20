/*
 *  Copyright (c) 2016, The OpenThread Authors.
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

#include "openthread-posix-config.h"
#include "platform-posix.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>

#include "common/code_utils.hpp"

static bool     sIsMsRunning = false;
static uint32_t sMsAlarm     = 0;

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
static bool     sIsUsRunning = false;
static uint32_t sUsAlarm     = 0;
#endif

static uint32_t sSpeedUpFactor = 1;

#ifdef __linux__

#include <signal.h>
#include <time.h>

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE && !OPENTHREAD_POSIX_VIRTUAL_TIME
static timer_t sMicroTimer;
static int     sRealTimeSignal = 0;

static void microTimerHandler(int aSignal, siginfo_t *aSignalInfo, void *aUserContext)
{
    assert(aSignal == sRealTimeSignal);
    assert(aSignalInfo->si_value.sival_ptr == &sMicroTimer);
    OT_UNUSED_VARIABLE(aSignal);
    OT_UNUSED_VARIABLE(aSignalInfo);
    OT_UNUSED_VARIABLE(aUserContext);
}
#endif // OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE && !OPENTHREAD_POSIX_VIRTUAL_TIME
#endif // __linux__

#ifdef CLOCK_MONOTONIC_RAW
#define OT_POSIX_CLOCK_ID CLOCK_MONOTONIC_RAW
#else
#define OT_POSIX_CLOCK_ID CLOCK_MONOTONIC
#endif

#if !OPENTHREAD_POSIX_VIRTUAL_TIME
uint64_t otPlatTimeGet(void)
{
    struct timespec now;

    VerifyOrDie(clock_gettime(OT_POSIX_CLOCK_ID, &now) == 0, OT_EXIT_FAILURE);

    return static_cast<uint64_t>(now.tv_sec) * US_PER_S + static_cast<uint64_t>(now.tv_nsec) / NS_PER_US;
}
#endif // !OPENTHREAD_POSIX_VIRTUAL_TIME

static uint64_t platformAlarmGetNow(void)
{
    return otPlatTimeGet() * sSpeedUpFactor;
}

void platformAlarmInit(uint32_t aSpeedUpFactor, int aRealTimeSignal)
{
    sSpeedUpFactor = aSpeedUpFactor;

    if (aRealTimeSignal == 0)
    {
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
        otLogWarnPlat("Real time signal not enabled, microsecond timers may be inaccurate!");
#endif
    }
#ifdef __linux__
    else if (aRealTimeSignal >= SIGRTMIN && aRealTimeSignal <= SIGRTMAX)
    {
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE && !OPENTHREAD_POSIX_VIRTUAL_TIME
        struct sigaction sa;
        struct sigevent  sev;

        sa.sa_flags     = SA_SIGINFO;
        sa.sa_sigaction = microTimerHandler;
        sigemptyset(&sa.sa_mask);

        VerifyOrDie(sigaction(aRealTimeSignal, &sa, nullptr) != -1, OT_EXIT_ERROR_ERRNO);

        sev.sigev_notify          = SIGEV_SIGNAL;
        sev.sigev_signo           = aRealTimeSignal;
        sev.sigev_value.sival_ptr = &sMicroTimer;

        VerifyOrDie(timer_create(CLOCK_MONOTONIC, &sev, &sMicroTimer) != -1, OT_EXIT_ERROR_ERRNO);

        sRealTimeSignal = aRealTimeSignal;
#endif // OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE && !OPENTHREAD_POSIX_VIRTUAL_TIME
    }
#endif // __linux__
    else
    {
        DieNow(OT_EXIT_INVALID_ARGUMENTS);
    }
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    return (uint32_t)(platformAlarmGetNow() / US_PER_MS);
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    sMsAlarm     = aT0 + aDt;
    sIsMsRunning = true;
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sIsMsRunning = false;
}

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
uint32_t otPlatAlarmMicroGetNow(void)
{
    return static_cast<uint32_t>(platformAlarmGetNow());
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    sUsAlarm     = aT0 + aDt;
    sIsUsRunning = true;

#ifdef __linux__
    if (sRealTimeSignal != 0)
    {
        struct itimerspec its;
        uint32_t          diff = sUsAlarm - otPlatAlarmMicroGetNow();

        its.it_value.tv_sec  = diff / US_PER_S;
        its.it_value.tv_nsec = (diff % US_PER_S) * NS_PER_US;

        its.it_interval.tv_sec  = 0;
        its.it_interval.tv_nsec = 0;

        if (-1 == timer_settime(sMicroTimer, 0, &its, nullptr))
        {
            otLogWarnPlat("Failed to update microsecond timer: %s", strerror(errno));
        }
    }
#endif // __linux__
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sIsUsRunning = false;

#ifdef __linux__
    if (sRealTimeSignal != 0)
    {
        struct itimerspec its = {{0, 0}, {0, 0}};

        if (-1 == timer_settime(sMicroTimer, 0, &its, nullptr))
        {
            otLogWarnPlat("Failed to stop microsecond timer: %s", strerror(errno));
        }
    }
#endif // __linux__
}
#endif // OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE

void platformAlarmUpdateTimeout(struct timeval *aTimeout)
{
    int64_t  remaining = INT32_MAX;
    uint64_t now       = platformAlarmGetNow();

    assert(aTimeout != nullptr);

    if (sIsMsRunning)
    {
        remaining = (int32_t)(sMsAlarm - (uint32_t)(now / US_PER_MS));
        VerifyOrExit(remaining > 0);
        remaining *= US_PER_MS;
        remaining -= (now % US_PER_MS);
    }

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    if (sIsUsRunning)
    {
        int32_t usRemaining = (int32_t)(sUsAlarm - (uint32_t)now);

        if (usRemaining < remaining)
        {
            remaining = usRemaining;
        }
    }
#endif // OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE

exit:
    if (remaining <= 0)
    {
        aTimeout->tv_sec  = 0;
        aTimeout->tv_usec = 0;
    }
    else
    {
        remaining /= sSpeedUpFactor;

        if (remaining == 0)
        {
            remaining = 1;
        }

        if (remaining < static_cast<int64_t>(aTimeout->tv_sec) * US_PER_S + static_cast<int64_t>(aTimeout->tv_usec))
        {
            aTimeout->tv_sec  = static_cast<time_t>(remaining / US_PER_S);
            aTimeout->tv_usec = static_cast<suseconds_t>(remaining % US_PER_S);
        }
    }
}

void platformAlarmProcess(otInstance *aInstance)
{
    int32_t remaining;

    if (sIsMsRunning)
    {
        remaining = (int32_t)(sMsAlarm - otPlatAlarmMilliGetNow());

        if (remaining <= 0)
        {
            sIsMsRunning = false;

#if OPENTHREAD_CONFIG_DIAG_ENABLE

            if (otPlatDiagModeGet())
            {
                otPlatDiagAlarmFired(aInstance);
            }
            else
#endif
            {
                otPlatAlarmMilliFired(aInstance);
            }
        }
    }

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE

    if (sIsUsRunning)
    {
        remaining = (int32_t)(sUsAlarm - otPlatAlarmMicroGetNow());

        if (remaining <= 0)
        {
            sIsUsRunning = false;

            otPlatAlarmMicroFired(aInstance);
        }
    }

#endif // OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
}
