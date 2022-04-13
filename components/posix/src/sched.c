/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <aos/kernel.h>
#include <aos/rhino.h>

#include "internal/pthread.h"
#include "internal/sched.h"

/* Note: pid is directly converted into pthread handle. */
static inline pthread_t _pid_to_pthread(pid_t pid)
{
    if (pid == -1) {
        return NULL;
    }

    return (pthread_t)pid;
}

/* Convert pid to ptcb of the thread. */
static inline pthread_tcb_t* sched_get_ptcb(pid_t pid)
{
    pthread_t thread  = 0;

    thread = _pid_to_pthread(pid);
    if (thread == 0) {
        thread = pthread_self();
    }

    return __pthread_get_tcb(thread);
}

int sched_yield(void)
{
    aos_task_yield();
    return 0;
}

int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;
    uint8_t priority = 0;
    int kpolicy = 0;
    uint8_t old_policy = 0;

    ptcb = sched_get_ptcb(pid);
    if (ptcb == NULL) {
        errno = ESRCH;
        return -1;
    }

    kpolicy = sched_policy_posix2rhino(policy);
    if (kpolicy == -1) {
        errno = EINVAL;
        return -1;
    }

    if ((param == NULL) || (param->sched_priority < aos_sched_get_priority_min(kpolicy))
        || (param->sched_priority > aos_sched_get_priority_max(kpolicy))) {
        errno = EINVAL;
        return -1;
    }

    priority = sched_priority_posix2rhino(kpolicy, param->sched_priority);

    ret = aos_task_sched_policy_get(&(ptcb->task), &old_policy);
    if (ret != 0) {
        return -1;
    }
    old_policy = sched_policy_rhino2posix(old_policy);

    /* Change the policy and priority of the thread */
    ret = aos_task_sched_policy_set(&(ptcb->task), kpolicy, priority);
    if ((ret == 0) && (kpolicy == KSCHED_RR)) {
        ret = aos_task_time_slice_set(&(ptcb->task), param->slice);
    }
    if (ret != 0) {
        return -1;
    }

    ptcb->attr.sched_priority = param->sched_priority;
    ptcb->attr.sched_slice = param->slice;

    return old_policy;
}

int sched_getscheduler(pid_t pid)
{
    int ret = 0;
    uint8_t policy = 0;
    pthread_tcb_t *ptcb = NULL;

    ptcb = sched_get_ptcb(pid);
    if (ptcb == NULL) {
        errno = ESRCH;
        return -1;
    }

    ret = aos_task_sched_policy_get(&(ptcb->task), &policy);
    if (ret != 0) {
        errno = EINVAL;
        return -1;
    }

    return sched_policy_rhino2posix(policy);
}

int sched_setparam(pid_t pid, const struct sched_param *param)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;
    uint8_t old_priority = 0;
    uint8_t priority = 0;
    uint8_t kpolicy = 0;

    ptcb = sched_get_ptcb(pid);
    if (ptcb == NULL) {
        errno = ESRCH;
        return -1;
    }

    ret = aos_task_sched_policy_get(&(ptcb->task), &kpolicy);
    if (ret != 0) {
        return -1;
    }

    if ((param == NULL) || (param->sched_priority < aos_sched_get_priority_min(kpolicy))
        || (param->sched_priority > aos_sched_get_priority_max(kpolicy))) {
        errno = EINVAL;
        return -1;
    }

    priority = sched_priority_posix2rhino(kpolicy, param->sched_priority);

    /* Change the priority of the thread. */
    ret = aos_task_pri_change(&(ptcb->task), priority, &old_priority);
    if (ret != 0) {
        return -1;
    }
    ptcb->attr.sched_priority = param->sched_priority;

    if (kpolicy == KSCHED_RR) {
        /* Change the time slice of the thread. */
        ret = aos_task_time_slice_set(&(ptcb->task), param->slice);
        if (ret != 0) {
            return -1;
        }
        ptcb->attr.sched_slice = param->slice;
    }

    return 0;
}

int sched_getparam(pid_t pid, struct sched_param *param)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;
    int kpolicy = 0;

    if (param == NULL) {
        errno = EINVAL;
        return -1;
    }

    ptcb = sched_get_ptcb(pid);
    if (ptcb == NULL) {
        errno = ESRCH;
        return -1;
    }

    ret = aos_task_pri_get(&(ptcb->task), (uint8_t *)&param->sched_priority);
    if (ret != 0){
        return -1;
    }

    ret = aos_task_sched_policy_get(&(ptcb->task), (uint8_t *)&kpolicy);
    if (ret != 0) {
        return -1;
    }
    param->sched_priority = sched_priority_rhino2posix(kpolicy, param->sched_priority);

    /* Slice should be 0 if that is not RR policy. */
    ret = aos_task_time_slice_get(&(ptcb->task), (uint32_t *)&param->slice);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int sched_get_priority_max(int policy)
{
    int ret = 0;

    ret = sched_policy_posix2rhino(policy);
    if (ret == -1) {
        errno = EINVAL;
        return -1;
    }

    ret = aos_sched_get_priority_max(ret);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int sched_get_priority_min(int policy)
{
    int ret = 0;

    ret = sched_policy_posix2rhino(policy);
    if (ret == -1) {
        errno = EINVAL;
        return -1;
    }

    ret = aos_sched_get_priority_min(ret);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }

    return ret;
}

int sched_rr_get_interval(pid_t pid, struct timespec *interval)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;
    uint8_t policy = 0;
    uint32_t slice_ms = 0;

    if (interval == NULL) {
        errno = EINVAL;
        return -1;
    }

    ptcb = sched_get_ptcb(pid);
    if (ptcb == NULL) {
        errno = ESRCH;
        return -1;
    }

    ret = aos_task_sched_policy_get(&(ptcb->task), &policy);
    if ((ret != 0) || (policy != KSCHED_RR)) {
        errno = EINVAL;
        return -1;
    }

    ret = aos_task_time_slice_get(&(ptcb->task), &slice_ms);
    if (ret != 0) {
        return -1;
    }

    interval->tv_sec = slice_ms / 1000;
    interval->tv_nsec = (slice_ms % 1000) * 1000000;

    return 0;
}
