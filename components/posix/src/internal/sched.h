/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef __POSIX_INTERNAL_SCHED_H
#define __POSIX_INTERNAL_SCHED_H

#include <sched.h>
#include <pthread.h>
#include <k_api.h>

/* Convert the schedule policy of posix to rhino. */
static inline int sched_policy_posix2rhino(int policy)
{
    switch (policy) {
        case SCHED_FIFO:
            return KSCHED_FIFO;
        case SCHED_RR:
            return KSCHED_RR;
        case SCHED_CFS:
            return KSCHED_CFS;
        default:
            return -1;
    }
}

/* Convert the schedule policy of rhino to posix. */
static inline int sched_policy_rhino2posix(int policy)
{
    switch (policy) {
        case KSCHED_FIFO:
            return SCHED_FIFO;
        case KSCHED_RR:
            return SCHED_RR;
        case KSCHED_CFS:
            return SCHED_CFS;
        default:
            return -1;
    }
}

/* In rhino: lower priority value means higher priority.
 * In posix standard: higher priority value means higher priority.
 */
static inline int sched_priority_posix2rhino(int policy, int priority)
{
    return aos_sched_get_priority_max(policy) - priority;
}

#define sched_priority_rhino2posix sched_priority_posix2rhino

#endif /*__POSIX_INTERNAL_SCHED_H*/
