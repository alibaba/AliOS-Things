
/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _SCHED_H
#define _SCHED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/types.h>
#include <sys/timespec.h>

/* The scheduling policy */
#define SCHED_OTHER 0
#define SCHED_FIFO  1
#define SCHED_RR    2
#define SCHED_CFS   3

struct sched_param {
    int      sched_priority; /* process execution scheduling priority */
    uint64_t slice;          /* time slice in SCHED_RR mode (ms) */
};

int sched_yield(void);
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
int sched_get_priority_min(int policy);
int sched_get_priority_max(int policy);
int sched_rr_get_interval(pid_t pid, struct timespec *interval);

#ifdef __cplusplus
}
#endif

#endif /*_SCHED_H*/
