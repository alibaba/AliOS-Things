/*
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#include <yoc_config.h>
#include <csi_kernel.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "soc.h"
#include <sys/types.h>
#include "csi_core.h"
#include "drv_timer.h"

#define NSEC_PER_SEC        1000000000
#define USEC_PER_SEC        1000000
#define NSEC_PER_USEC       1000
#define USEC_PER_MSEC       1000
#define MSEC_PER_SEC        1000
#define NSEC_PER_MSEC       1000000
#define TICK2MSEC(tick)     ((tick)* (1000 / CLOCKS_PER_SEC))

#ifndef CONFIG_LPM_TICKLESS_SLEEP
extern uint64_t g_sys_tick_count;
extern uint32_t csi_coret_get_value(void);
extern uint32_t csi_coret_get_load(void);
#else
extern uint32_t g_prev_cnt;
#endif

#ifndef CONFIG_CLOCK_BASETIME
#define CONFIG_CLOCK_BASETIME 1003939200
#endif
static struct timespec g_basetime = {
    .tv_sec = CONFIG_CLOCK_BASETIME,
    .tv_nsec = 0
};

static struct timespec last_readtime = {
    .tv_sec = 0,
    .tv_nsec = 0
};

/*
 * return : the coretim register count in a tick(calculate by TICK_PER_SEC)
 */

static uint32_t coretim_getpass(void)
{
    uint32_t cvalue;
    int      value;

#ifdef CONFIG_LPM_TICKLESS_SLEEP
    extern timer_handle_t count_timer;
    csi_timer_get_current_value(count_timer, &cvalue);
    value = ~cvalue - ~g_prev_cnt;
#else
    uint32_t loadtime;
    loadtime = csi_coret_get_load();
    cvalue = csi_coret_get_value();
    value = loadtime - cvalue;
#endif
    return value > 0 ? value : 0;
}

static int coretimspec(struct timespec *ts)
{
    uint32_t msecs;
    uint32_t secs;
    uint32_t nsecs = 0;

    if (ts == NULL) {
        return -1;
    }

    while (1) {
        uint64_t clk1, clk2;
        uint32_t pass1, pass2;

        clk1 = csi_kernel_get_ticks();
        pass1 = coretim_getpass();
        clk2 = csi_kernel_get_ticks();
        pass2 = coretim_getpass();

        if (clk1 == clk2 && pass2 > pass1) {
            msecs = TICK2MSEC(clk1);
            nsecs = pass2 * (NSEC_PER_SEC / SYSTEM_CLOCK);
            break;
        }
    }

    secs = msecs / MSEC_PER_SEC;

    nsecs += (msecs - (secs * MSEC_PER_SEC)) * NSEC_PER_MSEC;

    if (nsecs > NSEC_PER_SEC) {
        nsecs -= NSEC_PER_SEC;
        secs += 1;
    }

    ts->tv_sec = (time_t)secs;
    ts->tv_nsec = (long)nsecs;
    return OK;
}

int clock_gettime(clockid_t clockid, struct timespec *tp)
{
    struct timespec ts;
    uint32_t        carry;
    int             ret = OK;

    tp->tv_sec = 0;

    if (clockid == CLOCK_MONOTONIC) {
        ret = coretimspec(tp);
        if(ret < 0) {
            return -1;
        }
    }

    if (clockid == CLOCK_REALTIME) {
        ret = coretimspec(&ts);

        if (ret == OK) {

            if (ts.tv_nsec < last_readtime.tv_nsec) {
                ts.tv_nsec += NSEC_PER_SEC;
                ts.tv_sec -= 1;
            }

            carry = (ts.tv_nsec - last_readtime.tv_nsec) + g_basetime.tv_nsec;

            if (carry >= NSEC_PER_SEC) {
                carry -= NSEC_PER_SEC;
                tp->tv_sec += 1;
            }

            tp->tv_sec += (ts.tv_sec - last_readtime.tv_sec) + g_basetime.tv_sec;
            tp->tv_nsec = carry;
        }
    }

    return OK;
}

int clock_settime(clockid_t clockid, const struct timespec *tp)
{
    struct timespec bias;
    uint32_t      flags;
    int             ret = OK;

    if (clockid == CLOCK_REALTIME) {
        /* Interrupts are disabled here so that the in-memory time
         * representation and the RTC setting will be as close as
         * possible.
         */

        flags = csi_irq_save();

        /* Get the elapsed time since power up (in milliseconds).  This is a
         * bias value that we need to use to correct the base time.
         */

        coretimspec(&bias);
        g_basetime.tv_sec = tp->tv_sec;
        g_basetime.tv_nsec = tp->tv_nsec;
        last_readtime.tv_sec = bias.tv_sec;
        last_readtime.tv_nsec = bias.tv_nsec;
        csi_irq_restore(flags);

    } else {
        ret = -1;
    }

    return ret;
}

time_t time(time_t *tloc)
{
  struct timespec ts;
  int ret;

  /* Get the current time from the system */

  ret = clock_gettime(CLOCK_REALTIME, &ts);
  if (ret == 0)
    {
      /* Return the seconds since the epoch */

      if (tloc)
        {
          *tloc = ts.tv_sec;
        }

      return ts.tv_sec;
    }

  return (time_t)-1;
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    struct timespec ts;
    int             ret;

    if (!tv) {
        return -1;
    }

    ret = clock_gettime(CLOCK_REALTIME, &ts);

    if (ret == OK) {
        tv->tv_sec = ts.tv_sec;
        tv->tv_usec = ts.tv_nsec / NSEC_PER_USEC;
    }

    return ret;
}

int settimeofday(const struct timeval *tv, struct timezone *tz)
{
    struct timespec ts;

    if (!tv || tv->tv_usec >= USEC_PER_SEC) {
        return -1;
    }

    /* Convert the timeval to a timespec */

    ts.tv_sec  = tv->tv_sec;
    ts.tv_nsec = tv->tv_usec * NSEC_PER_USEC;

    /* Let clock_settime do the work */

    return clock_settime(CLOCK_REALTIME, &ts);
}

