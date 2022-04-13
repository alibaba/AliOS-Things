/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <pthread.h>
#include <timer.h>

int sched_yield(void)
{
    krhino_task_yield();

    return 0;
}

int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param)
{
    pthread_t       pth  = NULL;
    _pthread_tcb_t *ptcb = NULL;

    int ret     = -1;
    uint8_t old_pri = 0;
    uint8_t pri = 0;

    if (param == NULL) {
        return -1;
    }

    pth = _pid_to_pthread(pid);
    if (pth == NULL) {
        return -1;
    }

    ptcb = _pthread_get_tcb(pth);

    pri = PRI_CONVERT_PX_RH(param->sched_priority);

    if (policy == (int)SCHED_FIFO) {
        ret = krhino_sched_policy_set((ktask_t *)ptcb->tid, KSCHED_FIFO, pri);
    } else if (policy == (int)SCHED_RR) {
        ret = krhino_sched_policy_set((ktask_t *)ptcb->tid, KSCHED_RR, pri);
        if (ret == 0) {
            ret = krhino_task_time_slice_set((ktask_t *)ptcb->tid, (param->slice * 1000) / RHINO_CONFIG_TICKS_PER_SECOND);
        }
    } else if (policy == (int)SCHED_CFS) {
        ret = krhino_sched_policy_set((ktask_t *)ptcb->tid, KSCHED_CFS, pri);
    } else {
        return -1;
    }

    if (ret != RHINO_SUCCESS) {
        return -1;
    }

    /* change the priority of pthread */
    ret = krhino_task_pri_change((ktask_t *)ptcb->tid, PRI_CONVERT_PX_RH(param->sched_priority), &old_pri);
    if (ret != RHINO_SUCCESS) {
        return -1;
    }

    ptcb->attr.schedparam = *param;

    return 0;
}

int sched_get_priority_max(int policy)
{
    return POSIX_HIGH_PRI;
}

int sched_get_priority_min(int policy)
{
    return POSIX_LOW_PRI;
}

int sched_rr_get_interval(pid_t pid, struct timespec *interval)
{
    pthread_t pth      = NULL;
    uint32_t  ticks    = 0;
    uint64_t  value_ns = 0;
    _pthread_tcb_t *ptcb = NULL;


    if (interval == NULL) {
        return -1;
    }

    pth = _pid_to_pthread(pid);
    if (pth == NULL) {
        return -1;
    }

    ptcb = _pthread_get_tcb(pth);

    ticks = ((ktask_t *)ptcb->tid)->time_slice;
    value_ns = (ticks * NANOSECONDS_PER_SECOND) / RHINO_CONFIG_TICKS_PER_SECOND;
    interval->tv_sec = value_ns / NANOSECONDS_PER_SECOND;
    interval->tv_nsec = value_ns % NANOSECONDS_PER_SECOND;

    return 0;
}

