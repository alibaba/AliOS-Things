
/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef __POSIX_SCHED_H
#define __POSIX_SCHED_H

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

/* The scheduling policy */
#define SCHED_OTHER 0
#define SCHED_FIFO  1
#define SCHED_RR    2
#define SCHED_CFS   3

struct sched_param {
    int      sched_priority; /* process execution scheduling priority */
    uint64_t slice;          /* time slice in SCHED_RR mode (ms) */
};

#ifdef __cplusplus
extern "C" {
#endif

int sched_yield(void);
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
int sched_get_priority_min(int policy);
int sched_get_priority_max(int policy);
int sched_rr_get_interval(pid_t pid, struct timespec *interval);

#ifdef __cplusplus
}
#endif

#endif /*__POSIX_SCHED_H*/
