/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <time.h>

#include <aos/errno.h>
#include <aos/kernel.h>

int clock_getres(clockid_t clock_id, struct timespec *res)
{
    /* At now, only support CLOCK_REALTIME/CLOCK_MONOTONIC clock. */
    if (((clock_id != CLOCK_REALTIME) && (clock_id != CLOCK_MONOTONIC)) || (res == NULL)) {
        errno = EINVAL;
        return -1;
    }

    res->tv_sec = 0;
    res->tv_nsec = 1000000;

    return 0;
}

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    uint64_t time_ms = 0;

    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (clock_id == CLOCK_MONOTONIC) {
        time_ms = aos_now_ms();
        tp->tv_sec = time_ms / 1000;
        tp->tv_nsec = (time_ms % 1000) * 1000000;
    } else if (clock_id == CLOCK_REALTIME) {
        time_ms = aos_calendar_time_get();
        tp->tv_sec = time_ms / 1000;
        tp->tv_nsec = (time_ms % 1000) * 1000000;
    } else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
    uint64_t time_ms = 0;

    /* only CLOCK_REALTIME can be set */
    if ((clock_id != CLOCK_REALTIME) || (tp == NULL) ||
        (tp->tv_nsec < 0) || (tp->tv_nsec >= 1000000000UL)) {
        errno = EINVAL;
        return -1;
    }

    time_ms = (tp->tv_sec * 1000) + (tp->tv_nsec / 1000000);
    aos_calendar_time_set(time_ms);

    return 0;
}

int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *rqtp,
                    struct timespec *rmtp)
{
    int ret = 0;
    int64_t time_ms = 0;
    struct timespec tp = {0};

    (void)rmtp;

    if ((rqtp == NULL) || (rqtp->tv_sec < 0) || (rqtp->tv_nsec < 0)
                       || (rqtp->tv_nsec >= 1000000000UL)) {
        return EINVAL;
    }

    if ((clock_id != CLOCK_MONOTONIC) && (clock_id != CLOCK_REALTIME)) {
        return EINVAL;
    }

    if (flags == TIMER_ABSTIME) {
        /* absolute time */
        ret = clock_gettime(clock_id, &tp);
        if (ret != 0) {
            return ENOTSUP;
        }

        time_ms = (rqtp->tv_sec - tp.tv_sec) * 1000 + (rqtp->tv_nsec - tp.tv_nsec) / 1000000;
        if (time_ms <= 0) {
            /* The absolute time has passed, do not need to sleep. */
            return 0;
        }

    } else {
        /* relative time */
        time_ms = rqtp->tv_sec * 1000 + rqtp->tv_nsec / 1000000;
    }

    aos_msleep(time_ms);

    return 0;
}

/* Only support milliseconds resulation now by system tick. */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    uint64_t time_ms = 0;

    /* The rmtp parameter is not supported, because signal is not supported. */
    (void)rmtp;

    if ((rqtp == NULL) || (rqtp->tv_sec < 0) || (rqtp->tv_nsec < 0)
                       || (rqtp->tv_nsec >= 1000000000UL)) {
        errno = EINVAL;
        return -1;
    }

    time_ms = (rqtp->tv_sec * 1000) + (rqtp->tv_nsec / 1000000);
    aos_msleep(time_ms);

    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    struct timespec tv = { .tv_sec = seconds, .tv_nsec = 0 };

    if (nanosleep(&tv, &tv))
        return tv.tv_sec;

    return 0;
}

int usleep(useconds_t us)
{
    struct timespec tv = {
        .tv_sec = us / 1000000,
        .tv_nsec = (us % 1000000) * 1000
    };

    return nanosleep(&tv, &tv);
}
