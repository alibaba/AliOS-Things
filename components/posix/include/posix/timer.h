/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _TIMER_H
#define _TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "k_api.h"
#include "signal.h"

#define NANOSECONDS_PER_SECOND       1000000000LL
#define MICROSECONDS_PER_SECOND      1000000LL
#define MILLISECONDS_PER_SECOND      1000LL
#define NANOSECONDS_PER_MICROSECONDS 1000000LL

#undef CLOCK_REALTIME
#define CLOCK_REALTIME  0
#undef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1

int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid);
int timer_delete(timer_t timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
int timer_gettime(timer_t timerid, struct itimerspec *value);
int timer_getoverrun(timer_t timerid);

int clock_getres(clockid_t clock_id, struct timespec *res);
int clock_gettime(clockid_t clock_id, struct timespec *tp);
int clock_settime(clockid_t clock_id, const struct timespec *tp);
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp, struct timespec *rmtp);

unsigned int sleep(unsigned int seconds);
int usleep(useconds_t us);
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

#ifdef __cplusplus
}
#endif

#endif /* _TIMER_H */
