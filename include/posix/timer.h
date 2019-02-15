/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#include "k_api.h"
#include "posix/signal.h"
#include "posix_config.h"

#define NANOSECONDS_PER_SECOND       1000000000LL
#define MICROSECONDS_PER_SECOND      1000000LL
#define MILLISECONDS_PER_SECOND      1000LL
#define NANOSECONDS_PER_MICROSECONDS 1000000LL

#define CLOCK_REALTIME  1 /* System-wide realtime clock, can jump forwards and backwards as the system time-of-day clock is changed */
#define CLOCK_MONOTONIC 2 /* The absolute elapsed wall-clock time since some arbitrary, fixed point in the past. It isn't affected by
                             changes in the system time-of-day clock */

#define POSIX_TIMER_ID_MIN 1

typedef struct timer_list_s {
    timer_t   id;
    ktimer_t *ktimer;

    struct timer_list_s *next;
} timer_list_t;

int timer_create(clockid_t clockid, struct sigevent *restrict evp, timer_t *restrict timerid);
int timer_delete(timer_t timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *restrict value, struct itimerspec *restrict ovalue);
int timer_gettime(timer_t timerid, struct itimerspec *value);
int timer_getoverrun(timer_t timerid);

int clock_getres(clockid_t clock_id, struct timespec *res);
int clock_gettime(clockid_t clock_id, struct timespec *tp);
int clock_settime(clockid_t clock_id, const struct timespec *tp);
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp, struct timespec *rmtp);

unsigned int sleep(unsigned int seconds);
int          nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TIMER_H */
