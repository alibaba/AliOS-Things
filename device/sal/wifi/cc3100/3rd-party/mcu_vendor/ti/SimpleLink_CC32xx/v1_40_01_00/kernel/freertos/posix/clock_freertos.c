/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== clock_freertos.c ========
 */

#include <stdint.h>
#include "time.h"
#include "errno.h"

/* FreeRTOS header files */
#include <FreeRTOS.h>
#include <task.h>
#include <portmacro.h>

#include <ti/drivers/dpl/HwiP.h>

/*
 *  Use to keep track of system tick count rollovers.  The tick count is
 *  32 bits, so at a 1 msec tick rate, the count rolls over every
 *  4294967296 / 1000 seconds, or about every 50 days.
 *  By keeping track of tick rollovers, we can ensure that clock_gettime()
 *  is monotonic.
 *  To detect rollover, we'll save the current tick count in clock_gettime()
 *  to prevTicks.  If the current tick count is less than prevTicks, the
 *  tick count has rolled over and we'll increment rolloverCount. The
 *  current tick count not being less than prevTicks, does not necessarily
 *  mean that the count has not rolled over.  But this doesn't matter as
 *  far as ensuring that clock_gettime() is monotonic.
 */

/*
 *  Take the ceiling because we don't want to go back in time when the
 *  counter wraps.  Whatever number of ticks remaining will be counted
 *  as 1 second.
 */
#define SECONDS_PER_ROLLOVER (1 + 4294967295 / configTICK_RATE_HZ)

int _clock_abstime2ticks(clockid_t clockId, const struct timespec *abstime,
        TickType_t *ticks);

static uint32_t prevTicks = 0;
static uint32_t rolloverCount = 0;

/*
 *  ======== clock_gettime ========
 */
int clock_gettime(clockid_t clockId, struct timespec *ts)
{
    TickType_t      ticks;
    TickType_t      remTicks;
    unsigned long   secs;
    uint32_t        numRollovers;
    uintptr_t       key;

    key = HwiP_disable();

    /* Number of ticks since vTaskStartScheduler() was called */
    ticks = xTaskGetTickCount();

    if (ticks < prevTicks) {
        rolloverCount++;
    }
    prevTicks = ticks;
    numRollovers = rolloverCount;

    HwiP_restore(key);

    secs = ticks / configTICK_RATE_HZ;
    remTicks = ticks - secs * configTICK_RATE_HZ;

    ts->tv_sec = secs + numRollovers * SECONDS_PER_ROLLOVER;
    ts->tv_nsec = remTicks * (1000000000 / configTICK_RATE_HZ);

    return (0);
}

/*
 *  ======== clock_nanosleep ========
 */
int clock_nanosleep(clockid_t clock_id, int flags,
        const struct timespec *rqtp, struct timespec *rmtp)
{
    TickType_t xDelay;

    /* Ignore rmtp (remaining time, if woken by a signal) */
    if (rmtp) {
        rmtp->tv_sec = rmtp->tv_nsec = 0;
    }

    if (flags & TIMER_ABSTIME) {
        if (_clock_abstime2ticks(clock_id, rqtp, &xDelay) != 0) {
            return (EINVAL);
        }

        if (xDelay == 0) {
            return (0);
        }
    }
    else {
        if (rqtp->tv_sec >= SECONDS_PER_ROLLOVER - 1) {
            return (EINVAL);
        }

        if ((rqtp->tv_sec == 0) && (rqtp->tv_nsec == 0)) {
            return (0);
        }

        xDelay = rqtp->tv_sec * configTICK_RATE_HZ;

        /*
         *  Take the ceiling.  The remaining nanoseconds will count as an
         *  entire tick.
         */
        xDelay += (rqtp->tv_nsec + (1000000000 / configTICK_RATE_HZ) - 1) /
                (1000000000 / configTICK_RATE_HZ);

        /*
         *  Add one tick to ensure the timeout is not less than the
         *  amount of time requested.  The clock may be about to tick,
         *  and that counts as one tick even though the amount of time
         *  until this tick is less than the tick period.
         */
        xDelay++;
    }

    vTaskDelay(xDelay);

    return (0);
}

/*
 *  ======== clock_settime ========
 */
int clock_settime(clockid_t clock_id, const struct timespec *ts)
{
    return (-1);
}

/*
 *************************************************************************
 *              internal functions
 *************************************************************************
 */

/*
 *  ======== _clock_abstime2ticks ========
 */
int _clock_abstime2ticks(clockid_t clockId, const struct timespec *abstime,
        TickType_t *ticks)
{
    struct timespec    curtime;
    TickType_t         timeout;
    long               nsecs = 0;
    time_t             secs = 0;

    if ((abstime->tv_nsec < 0) || (1000000000 <= abstime->tv_nsec)) {
        return (-1);
    }
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    secs = abstime->tv_sec - curtime.tv_sec;

    if ((abstime->tv_sec < curtime.tv_sec) ||
            ((secs == 0) && (abstime->tv_nsec <= curtime.tv_nsec))) {
        timeout = 0;
    }
    else {
        nsecs = abstime->tv_nsec - curtime.tv_nsec;

        if (nsecs < 0) {
            nsecs += 1000000000;
            secs--;
        }

        timeout = secs * configTICK_RATE_HZ;
        timeout += (nsecs + (1000000000 / configTICK_RATE_HZ) - 1) /
            (1000000000 / configTICK_RATE_HZ);

        /*
         *  Add one tick to ensure the timeout is not less than the
         *  amount of time requested.  The clock may be about to tick,
         *  and that counts as one tick even though the amount of time
         *  until this tick is less than the clock period.
         */
        timeout++;
    }

    *ticks = timeout;

    return (0);
}


/*
 *  ======== _clock_timespecToTicks ========
 */
TickType_t _clock_timespecToTicks(const struct timespec *ts)
{
    uint64_t ticks;

    ticks = ts->tv_sec * configTICK_RATE_HZ +
            ts->tv_nsec * (uint64_t)configTICK_RATE_HZ / 1000000000L;
    return ((TickType_t)ticks);
}
