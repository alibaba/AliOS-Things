/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sched.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include <aos/rhino.h>
#include <posix/timer.h>

#include "internal/pthread.h"
#include "internal/sched.h"

pthread_mutex_t g_pthread_lock = PTHREAD_MUTEX_INITIALIZER;

int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void))
{
    /* Fork is not supported. */
    return ENOSYS;
}

void pthread_cleanup_pop(int execute)
{
    pthread_tcb_t *ptcb = NULL;
    pthread_cleanup_t *cleanup = NULL;

    ptcb = __pthread_get_tcb(pthread_self());
    if (ptcb == NULL) {
        return;
    }

    cleanup = ptcb->cleanup;
    if (cleanup != NULL) {
        ptcb->cleanup = cleanup->prev;
        if (execute != 0) {
            cleanup->cleanup_routine(cleanup->para);
        }
        free(cleanup);
    }
}

void pthread_cleanup_push(void (*routine)(void *), void *arg)
{
    pthread_tcb_t *ptcb = NULL;
    pthread_cleanup_t *cleanup = NULL;

    ptcb = __pthread_get_tcb(pthread_self());
    if (ptcb == NULL) {
        return;
    }

    cleanup = (pthread_cleanup_t *) malloc(sizeof(pthread_cleanup_t));
    if (cleanup != NULL) {
        cleanup->cleanup_routine = routine;
        cleanup->para = arg;
        cleanup->prev = ptcb->cleanup;
        ptcb->cleanup = cleanup;
    }
}

static void do_pthread_cleanup(pthread_tcb_t *ptcb)
{
    pthread_cleanup_t *cleanup = NULL;

    /* Execute all existed cleanup functions and free it. */
    do {
        cleanup = ptcb->cleanup;
        if (cleanup != NULL) {
            ptcb->cleanup = cleanup->prev;
            cleanup->cleanup_routine(cleanup->para);
            free(cleanup);
        }
    } while (ptcb->cleanup != NULL);
}

/* Exit the pthread, never return. */
void pthread_exit(void* value_ptr)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;

    ptcb = __pthread_get_tcb(pthread_self());
    if (ptcb == NULL) {
        return;
    }

    ptcb->return_value = value_ptr;

    /* Run cleanup functions of the thread */
    do_pthread_cleanup(ptcb);

    /* Run destructor functions of the thread */
    pthread_tsd_dtors();

    /* The underlying task in kernel is not available, unlink it from ptcb. */
    aos_task_ptcb_set(&(ptcb->task), NULL);
    ptcb->task = NULL;

    if (ptcb->attr.detachstate == PTHREAD_CREATE_JOINABLE) {
        /* Give join sem if is joinable */
        aos_sem_signal(&(ptcb->join_sem));
    } else if (ptcb->attr.detachstate == PTHREAD_CREATE_DETACHED) {
        /* Free the join sem */
        aos_sem_free(&(ptcb->join_sem));

        /* The user/kernel task stack and task tcb should be freed by kernel, here we
         * only free the ptcb.
         */
        ptcb->magic = 0;
        free(ptcb);
    } else {
        ;
    }

    /* Exit the task, never return. */
    aos_task_exit(0);
}

static void start_pthread(void *arg)
{
    pthread_tcb_t *ptcb = arg;
    pthread_exit(ptcb->thread_entry(ptcb->thread_para));
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg)
{
    int ret = 0;
    pthread_tcb_t *ptcb   = NULL;  /* The ptcb of new thread. */
    pthread_tcb_t *ptcb_c = NULL;  /* The ptcb of current thread. */
    uint32_t stack_size = 0;
    int kpolicy = 0;
    int kpriority = 0;

    if ((thread == NULL) || ((attr != NULL) && (attr->flag != PTHREAD_DYN_INIT))
                        || (start_routine == NULL)) {
        return EINVAL;
    }
    /* Init the pthread handle as NULL. */
    *thread = NULL;

    /* Create ptcb for the new thread. */
    ptcb = (pthread_tcb_t *)malloc(sizeof(pthread_tcb_t));
    if (ptcb == NULL) {
        return ENOMEM;
    }
    memset(ptcb, 0, sizeof(pthread_tcb_t));
    ptcb->magic = PTHREAD_TCB_MAGIC;

    if (attr != NULL) {
        ptcb->attr = *attr;
        if (attr->inheritsched == PTHREAD_INHERIT_SCHED) {
            ptcb_c = __pthread_get_tcb(pthread_self());
            if (ptcb_c != NULL) {
                ptcb->attr = ptcb_c->attr;
            }
        }
    } else {
        pthread_attr_init(&(ptcb->attr));
    }

    /* Init joinable semaphore. */
    if (ptcb->attr.detachstate == PTHREAD_CREATE_JOINABLE) {
        ret = aos_sem_new(&(ptcb->join_sem), 0);
        if (ret != 0) {
            ret = -1;
            goto out3;
        }
    }

    ptcb->thread_entry = start_routine;
    ptcb->thread_para  = arg;

    strncpy(ptcb->thread_name, "posix_thread", PTHREAD_NAME_MAX_LEN);
    kpolicy = aos_task_sched_policy_get_default();
    kpriority = sched_priority_posix2rhino(kpolicy, ptcb->attr.sched_priority);
    if (kpriority < 0) {
        ret = -1;
        goto out2;
    }
    ret = aos_task_create(&(ptcb->task), ptcb->thread_name, start_pthread,
                            ptcb, NULL, ptcb->attr.stacksize, kpriority, 0);
    if (ret != 0) {
        ret = -1;
        goto out2;
    }

    /* Store ptcb in kernel task tcb, and get it back when call pthread_self. */
    ret = aos_task_ptcb_set(&(ptcb->task), ptcb);
    if (ret != 0) {
        ret = -1;
        goto out1;
    }

    /* Success. */
    *thread = ptcb;

    ret = aos_task_resume(&(ptcb->task));
    if (ret != 0) {
        ret = -1;
        *thread = NULL;
        goto out1;
    }

    return 0;

out1:
    aos_task_delete(&(ptcb->task));
out2:
    aos_sem_free(&(ptcb->join_sem));
out3:
    if (ptcb != NULL) {
        ptcb->magic = 0;
        free(ptcb);
    }
    return ret;
}

int pthread_detach(pthread_t thread)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;

    if (thread == NULL) {
        return EINVAL;
    }

    ptcb = __pthread_get_tcb(thread);
    if (ptcb == NULL) {
        return EINVAL;
    }

    if (ptcb->attr.detachstate == PTHREAD_CREATE_DETACHED) {
        return EINVAL;
    } else {
        ret = pthread_mutex_lock(&g_pthread_lock);
        if (ret != 0) {
            return EAGAIN;
        }
        ptcb->attr.detachstate = PTHREAD_CREATE_DETACHED;
        pthread_mutex_unlock(&g_pthread_lock);
    }

    return 0;
}

int pthread_timedjoin_np(pthread_t thread, void **retval, const struct timespec *abstime)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;
    struct timeval time_now = {0};
    uint64_t msec = AOS_WAIT_FOREVER;
    uint64_t nsec = 0;
    int64_t sec = 0;

    if (thread == NULL) {
        return EINVAL;
    }
    ptcb = __pthread_get_tcb(thread);
    if (ptcb == NULL) {
        return EINVAL;
    }

    if (ptcb == pthread_self()) {
        return EINVAL;
    }

    if (ptcb->attr.detachstate != PTHREAD_CREATE_JOINABLE) {
        return EINVAL;
    }

    /* Get the time to wait. */
    if (abstime != NULL) {
        gettimeofday(&time_now, NULL);

        if ((time_now.tv_usec * 1000) > abstime->tv_nsec) {
            nsec = abstime->tv_nsec + 1000000000 - time_now.tv_usec * 1000;
            sec = abstime->tv_sec - time_now.tv_sec - 1;
        } else {
            nsec = abstime->tv_nsec - time_now.tv_usec * 1000;
            sec = abstime->tv_sec - time_now.tv_sec;
        }

        if (sec < 0) {
            return EINVAL;
        }

        msec = sec * 1000 + nsec / 1000000;
    }

    ret = aos_sem_wait(&(ptcb->join_sem), msec);
    if (ret == 0) {
        if (retval != NULL) {
            *retval = ptcb->return_value;
        }
        /* The task tcb struct and user/kernel stack should be freed by kernel when task delete */
        aos_sem_free(&(ptcb->join_sem));
        ptcb->magic = 0;
        free(ptcb);
    } else if (ret == -ETIMEDOUT) {
        return ETIMEDOUT;
    } else {
        return -1;
    }

    return 0;
}

int pthread_join(pthread_t thread, void **retval)
{

    return pthread_timedjoin_np(thread, retval, NULL);
}

int pthread_cancel(pthread_t thread)
{
    return ENOSYS;
}

void pthread_testcancel(void)
{
    return;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    return ENOSYS;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    return ENOSYS;
}

int pthread_kill(pthread_t thread, int sig)
{
    return ENOSYS;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return (int)(t1 == t2);
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;
    uint8_t priority = 0;
    unsigned int slice = 0;
    int kpolicy = 0;

    if (policy == NULL || param == NULL) {
        return EINVAL;
    }

    ptcb = __pthread_get_tcb(thread);
    if (ptcb == NULL) {
        return ESRCH;
    }

    ret = aos_task_sched_policy_get(&(ptcb->task), (uint8_t *)&kpolicy);
    if (ret != 0) {
        return EINVAL;
    }

    ret = aos_task_pri_get(&(ptcb->task), &priority);
    if (ret != 0){
        return EINVAL;
    }

    /* Slice should be 0 if that is not RR policy. */
    ret = aos_task_time_slice_get(&(ptcb->task), (uint32_t *) &slice);
    if (ret != 0) {
        return EINVAL;
    }

    *policy = sched_policy_rhino2posix(kpolicy);
    param->sched_priority = sched_priority_rhino2posix(kpolicy, priority);
    param->slice = slice;

    return 0;
}

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;
    uint8_t old_priority = 0;
    uint8_t priority = 0;
    int kpolicy = 0;

    ptcb = __pthread_get_tcb(thread);
    if (ptcb == NULL) {
        return EINVAL;
    }

    kpolicy = sched_policy_posix2rhino(policy);
    if (kpolicy == -1) {
        return EINVAL;
    }

    if ((param == NULL) || (param->sched_priority < aos_sched_get_priority_min(kpolicy))
        || (param->sched_priority > aos_sched_get_priority_max(kpolicy))) {
        return EINVAL;
    }

    priority = sched_priority_posix2rhino(kpolicy, param->sched_priority);

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

    return 0;
}

pthread_t pthread_self(void)
{
    int ret = 0;
    pthread_tcb_t *ptcb = NULL;
    aos_task_t task;

    task = aos_task_self();
    aos_task_ptcb_get(&task, (void **)&ptcb);
    if (ptcb == NULL) {
        /* Create ptcb for native task in case that call pthread_self */
        ptcb = (pthread_tcb_t *)malloc(sizeof(pthread_tcb_t));
        if (ptcb == NULL) {
            return NULL;
        }
        memset(ptcb, 0, sizeof(pthread_tcb_t));

        ptcb->magic = PTHREAD_TCB_MAGIC;
        ptcb->task = task;

        /* Set ptcb in task tcb. */
        ret = aos_task_ptcb_set(&(ptcb->task), ptcb);
        if (ret != 0) {
            ptcb->magic = 0;
            free(ptcb);
            return NULL;
        }
    }

    return ptcb;
}

int pthread_setschedprio(pthread_t thread, int prio)
{
    int ret = 0;
    pthread_tcb_t * ptcb = NULL;
    uint8_t old_prio = 0;
    int kpolicy = 0;
    int kpriority = 0;

    ptcb = __pthread_get_tcb(thread);
    if (ptcb == NULL) {
        return EINVAL;
    }

    ret = aos_task_sched_policy_get(&(ptcb->task), (uint8_t *)&kpolicy);
    if (ret != 0) {
        return -1;
    }

    kpriority = sched_priority_posix2rhino(kpolicy, prio);
    if (kpriority < 0) {
        return -1;
    }

    ret = aos_task_pri_change(&(ptcb->task), kpriority, &old_prio);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    int ret = 0;

    ret = pthread_mutex_lock(&g_pthread_lock);
    if (ret != 0) {
        return EAGAIN;
    }

    if (*once_control == PTHREAD_ONCE_INIT)
    {
        *once_control = !PTHREAD_ONCE_INIT;

        pthread_mutex_unlock(&g_pthread_lock);

        init_routine ();
        return 0;
    }

    pthread_mutex_unlock(&g_pthread_lock);

    return 0;
}

int pthread_getcpuclockid(pthread_t thread, clockid_t *clock_id)
{
    if ((thread == NULL) || (clock_id == NULL)) {
        return EINVAL;
    }

    *clock_id = CLOCK_MONOTONIC;

    return 0;
}

int pthread_getconcurrency(void)
{
    /* User thread and kernel thread are one-to-one correspondence in AliOS Things,
       so the concurrency is 0  */
    return 0;
}

int pthread_setconcurrency(int new_level)
{
    /* User thread and kernel thread are one-to-one correspondence in AliOS Things,
       so the concurrency can not be set  */
    return ENOSYS;
}

int pthread_setname_np(pthread_t thread, const char *name)
{
    pthread_tcb_t *ptcb = NULL;

    if ((thread == NULL) || (name == NULL)) {
        return EINVAL;
    }

    ptcb = __pthread_get_tcb(thread);
    if (ptcb == NULL) {
        return EINVAL;
    }

    /* Truncate the name if it's too long. */
    strncpy(ptcb->thread_name, name, PTHREAD_NAME_MAX_LEN);
    if (strlen(name) > PTHREAD_NAME_MAX_LEN) {
        return ERANGE;
    }

    return 0;
}

int pthread_getname_np(pthread_t thread, char *name, size_t len)
{
    pthread_tcb_t *ptcb = NULL;

    if ((thread == NULL) || (name == NULL)) {
        return EINVAL;
    }

    if (len < PTHREAD_NAME_MAX_LEN) {
        return ERANGE;
    }

    ptcb = __pthread_get_tcb(thread);
    if (ptcb == NULL) {
        return EINVAL;
    }

    memset(name, 0, len);
    strncpy(name, ptcb->thread_name, len);

    return 0;
}
