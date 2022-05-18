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

#include "platform-simulation.h"

#if OPENTHREAD_SIMULATION_VIRTUAL_TIME == 0

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "utils/code_utils.h"

#ifdef __linux__
#include <signal.h>
#include <time.h>

#ifndef OPENTHREAD_CONFIG_MICRO_TIMER_SIGNAL
#define OPENTHREAD_CONFIG_MICRO_TIMER_SIGNAL SIGRTMIN
#endif

timer_t sMicroTimer;
#endif // __linux__

#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>

#include "core/common/logging.hpp"
#include "lib/platform/exit_code.h"

#define MS_PER_S 1000
#define NS_PER_US 1000
#define US_PER_MS 1000
#define US_PER_S 1000000

#define DEFAULT_TIMEOUT_IN_SEC 10 // seconds

#ifdef CLOCK_MONOTONIC_RAW
#define OT_SIMULATION_CLOCK_ID CLOCK_MONOTONIC_RAW
#else
#define OT_SIMULATION_CLOCK_ID CLOCK_MONOTONIC
#endif

static bool     sIsMsRunning = false;
static uint32_t sMsAlarm     = 0;

static bool     sIsUsRunning = false;
static uint32_t sUsAlarm     = 0;

static uint32_t sSpeedUpFactor = 1;

#ifdef __linux__
static void microTimerHandler(int aSignal, siginfo_t *aSignalInfo, void *aUserContext)
{
    assert(aSignal == OPENTHREAD_CONFIG_MICRO_TIMER_SIGNAL);
    assert(aSignalInfo->si_value.sival_ptr == &sMicroTimer);
    (void)aSignal;
    (void)aSignalInfo;
    (void)aUserContext;
}
#endif

static bool isExpired(uint32_t aTime, uint32_t aNow)
{
    // Determine whether or not `aTime` is before or same as `aNow`.

    uint32_t diff = aNow - aTime;

    return (diff & (1U << 31)) == 0;
}

static uint32_t calculateDuration(uint32_t aTime, uint32_t aNow)
{
    // Return the time duration from `aNow` to `aTime` if `aTimer` is
    // after `aNow`, otherwise return zero.

    return isExpired(aTime, aNow) ? 0 : aTime - aNow;
}

void platformAlarmInit(uint32_t aSpeedUpFactor)
{
    sSpeedUpFactor = aSpeedUpFactor;

#ifdef __linux__
    {
        struct sigaction sa;

        sa.sa_flags     = SA_SIGINFO;
        sa.sa_sigaction = microTimerHandler;
        sigemptyset(&sa.sa_mask);

        if (sigaction(OPENTHREAD_CONFIG_MICRO_TIMER_SIGNAL, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }

        struct sigevent sev;

        sev.sigev_notify          = SIGEV_SIGNAL;
        sev.sigev_signo           = OPENTHREAD_CONFIG_MICRO_TIMER_SIGNAL;
        sev.sigev_value.sival_ptr = &sMicroTimer;

        if (-1 == timer_create(CLOCK_MONOTONIC, &sev, &sMicroTimer))
        {
            perror("timer_create");
            exit(EXIT_FAILURE);
        }
    }
#endif
}

#if defined(CLOCK_MONOTONIC_RAW) || defined(CLOCK_MONOTONIC)
uint64_t platformGetNow(void)
{
    struct timespec now;
    int             err;

    err = clock_gettime(OT_SIMULATION_CLOCK_ID, &now);

    VerifyOrDie(err == 0, OT_EXIT_ERROR_ERRNO);

    return (uint64_t)now.tv_sec * sSpeedUpFactor * US_PER_S + (uint64_t)now.tv_nsec * sSpeedUpFactor / NS_PER_US;
}
#else
uint64_t platformGetNow(void)
{
    struct timeval tv;
    int            err;

    err = gettimeofday(&tv, NULL);

    assert(err == 0);

    return (uint64_t)tv.tv_sec * sSpeedUpFactor * US_PER_S + (uint64_t)tv.tv_usec * sSpeedUpFactor;
}
#endif // defined(CLOCK_MONOTONIC_RAW) || defined(CLOCK_MONOTONIC)

uint32_t otPlatAlarmMilliGetNow(void)
{
    return (uint32_t)(platformGetNow() / US_PER_MS);
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

uint32_t otPlatAlarmMicroGetNow(void)
{
    return (uint32_t)platformGetNow();
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    sUsAlarm     = aT0 + aDt;
    sIsUsRunning = true;

#ifdef __linux__
    {
        struct itimerspec its;
        uint32_t          diff = sUsAlarm - otPlatAlarmMicroGetNow();

        its.it_value.tv_sec  = diff / US_PER_S;
        its.it_value.tv_nsec = (diff % US_PER_S) * NS_PER_US;

        its.it_interval.tv_sec  = 0;
        its.it_interval.tv_nsec = 0;

        if (-1 == timer_settime(sMicroTimer, 0, &its, NULL))
        {
            perror("otPlatAlarmMicroStartAt timer_settime()");
            exit(EXIT_FAILURE);
        }
    }
#endif // __linux__
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    sIsUsRunning = false;

#ifdef __linux__
    {
        struct itimerspec its = {{0, 0}, {0, 0}};

        if (-1 == timer_settime(sMicroTimer, 0, &its, NULL))
        {
            perror("otPlatAlarmMicroStop timer_settime()");
            exit(EXIT_FAILURE);
        }
    }
#endif // __linux__
}

void platformAlarmUpdateTimeout(struct timeval *aTimeout)
{
    uint64_t remaining = DEFAULT_TIMEOUT_IN_SEC * US_PER_S; // in usec.

    assert(aTimeout != NULL);

    if (sIsMsRunning)
    {
        uint32_t msRemaining = calculateDuration(sMsAlarm, otPlatAlarmMilliGetNow());

        remaining = ((uint64_t)msRemaining) * US_PER_MS;
    }

    if (sIsUsRunning)
    {
        uint32_t usRemaining = calculateDuration(sUsAlarm, otPlatAlarmMicroGetNow());

        if (usRemaining < remaining)
        {
            remaining = usRemaining;
        }
    }

    if (remaining == 0)
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

        aTimeout->tv_sec  = (time_t)remaining / US_PER_S;
        aTimeout->tv_usec = remaining % US_PER_S;
    }
}

void platformAlarmProcess(otInstance *aInstance)
{
    if (sIsMsRunning && isExpired(sMsAlarm, otPlatAlarmMilliGetNow()))
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

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    if (sIsUsRunning && isExpired(sUsAlarm, otPlatAlarmMicroGetNow()))
    {
        sIsUsRunning = false;

        otPlatAlarmMicroFired(aInstance);
    }
#endif
}

uint64_t otPlatTimeGet(void)
{
    return platformGetNow();
}

uint16_t otPlatTimeGetXtalAccuracy(void)
{
    return 0;
}

#endif // OPENTHREAD_SIMULATION_VIRTUAL_TIME == 0
