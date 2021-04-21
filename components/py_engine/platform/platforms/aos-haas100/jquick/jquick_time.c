//
// Copyright (c) 2020 Alibaba All rights reserved.
//

#include "jquick_time.h"
#ifdef __STANDARD_TIMER__
#include <time.h>
#else
#include <posix/timer.h>
#endif

long long jquick_get_current_time()
{
    struct timespec t;

    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);

    long long v = (t.tv_sec) * 1000 + t.tv_nsec / 1000000;

    return v;
}
