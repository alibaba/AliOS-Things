/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <aos/kernel.h>
#include <posix/timer.h>

#include "internal/pthread.h"

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    int ret = 0;

    if (cond == NULL) {
        return EINVAL;
    }

    memset(cond, 0, sizeof(pthread_cond_t));
    ret = aos_mutex_new((aos_mutex_t*)(&(cond->lock)));
    if (ret != 0) {
        return EAGAIN;
    }

    ret = aos_sem_new((aos_sem_t*)(&(cond->wait_sem)), 0);
    if (ret != 0) {
        aos_mutex_free((aos_mutex_t*)(&(cond->lock)));
        return EAGAIN;
    }

    ret = aos_sem_new((aos_sem_t*)(&(cond->wait_done)), 0);
    if (ret != 0) {
        aos_mutex_free((aos_mutex_t*)(&(cond->lock)));
        aos_sem_free((aos_sem_t*)(&(cond->wait_sem)));
        return EAGAIN;
    }

    cond->waiting = cond->signals = 0;
    cond->flag = PTHREAD_DYN_INIT;

    if (attr != NULL) {
        cond->attr = *attr;
    }

    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    if (cond == NULL) {
        return EINVAL;
    }

    aos_mutex_free((aos_mutex_t*)(&(cond->lock)));
    aos_sem_free((aos_sem_t*)(&(cond->wait_sem)));
    aos_sem_free((aos_sem_t*)(&(cond->wait_done)));

    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    int i           = 0;
    int num_waiting = 0;

    /* If there are waiting threads not already signalled, then
     * signal the condition and wait for the thread to respond.
     */
    aos_mutex_lock((aos_mutex_t*)(&(cond->lock)), AOS_WAIT_FOREVER);

    if (cond->waiting > cond->signals) {
        num_waiting = (cond->waiting - cond->signals);
        cond->signals = cond->waiting;

        for (i = 0; i < num_waiting; ++i) {
            aos_sem_signal((aos_sem_t*)(&(cond->wait_sem)));
        }
        /* Now all released threads are blocked here, waiting for us.
         * Collect them all (and win fabulous prizes!) :-)
         */
        aos_mutex_unlock((aos_mutex_t*)(&(cond->lock)));
        for (i = 0; i < num_waiting; ++i) {
            aos_sem_wait((aos_sem_t*)(&(cond->wait_done)), AOS_WAIT_FOREVER);
        }
    } else {
        aos_mutex_unlock((aos_mutex_t*)(&(cond->lock)));
    }

    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    /* If there are waiting threads not already signalled, then
     * signal the condition and wait for the thread to respond.
     */
    aos_mutex_lock((aos_mutex_t*)(&(cond->lock)), AOS_WAIT_FOREVER);
    if ( cond->waiting > cond->signals ) {
        (cond->signals)++;
        aos_sem_signal((aos_sem_t*)(&(cond->wait_sem)));
        aos_mutex_unlock((aos_mutex_t*)(&(cond->lock)));
        aos_sem_wait((aos_sem_t*)(&(cond->wait_done)), AOS_WAIT_FOREVER);
    } else {
        aos_mutex_unlock((aos_mutex_t*)(&(cond->lock)));
    }

    return 0;
}

static int pthread_cond_timedwait_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, int64_t ms)
{
    int ret = 0;

    /* Obtain the protection mutex, and increment the number of waiters.
     * This allows the signal mechanism to only perform a signal if there
     * are waiting threads.
     */
    aos_mutex_lock((aos_mutex_t*)(&(cond->lock)), AOS_WAIT_FOREVER);
    (cond->waiting)++;
    aos_mutex_unlock((aos_mutex_t*)(&(cond->lock)));

    /* Unlock the mutex, as is required by condition variable semantics */
    aos_mutex_unlock((aos_mutex_t*)(&(mutex->mutex)));

    /* Wait for a signal */
    ret = aos_sem_wait((aos_sem_t*)(&(cond->wait_sem)), ms);

    /* Let the signaler know we have completed the wait, otherwise
     * the signaler can race ahead and get the condition semaphore
     * if we are stopped between the mutex unlock and semaphore wait,
     * giving a deadlock.
     */
    aos_mutex_lock((aos_mutex_t*)(&(cond->lock)), AOS_WAIT_FOREVER);

    if (ret == -ETIMEDOUT) {
        ret = ETIMEDOUT;
    }

    if (cond->signals > 0) {
        /* If we timed out, we need to eat a condition signal */
        if (ret == ETIMEDOUT) {
            aos_sem_wait((aos_sem_t*)(&(cond->wait_sem)), AOS_NO_WAIT);
        }
        /* We always notify the signal thread that we are done */
        aos_sem_signal((aos_sem_t*)(&(cond->wait_done)));

        /* Signal handshake complete */
        (cond->signals)--;
    }

    (cond->waiting)--;

    aos_mutex_unlock((aos_mutex_t*)(&(cond->lock)));

    /* Lock the mutex, as is required by condition variable semantics */
    aos_mutex_lock((aos_mutex_t*)(&(mutex->mutex)), AOS_WAIT_FOREVER);

    return ret;
}


int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *abstime)
{
    int ret;
    struct timeval now_tv = {0};
    struct timespec now_tp = {0};
    int64_t timeout_ms = 0;

    if ((cond == NULL) || (mutex == NULL)) {
        return EINVAL;
    }

    if (abstime == NULL) {
        timeout_ms = AOS_WAIT_FOREVER;
    } else {
        if (cond->attr.clock == CLOCK_MONOTONIC) {
            ret = clock_gettime(CLOCK_MONOTONIC, &now_tp);
            if (ret != 0) {
                return EINVAL;
            }

            timeout_ms = (abstime->tv_sec - now_tp.tv_sec) * 1000 +
                         ((abstime->tv_nsec - now_tp.tv_nsec) / 1000000);
        } else {
            /* CLOCK_REALTIME */
            gettimeofday(&now_tv, NULL);
            timeout_ms = (abstime->tv_sec - now_tv.tv_sec) * 1000 +
                         ((abstime->tv_nsec - now_tv.tv_usec * 1000) / 1000000);
        }

        if (timeout_ms <= 0) {
            /* The absolute time has already passed. Do not need to wait. */
            return ETIMEDOUT;
        }
    }

    return pthread_cond_timedwait_ms(cond, mutex, timeout_ms);
}

int pthread_cond_timedwait_relative_np(pthread_cond_t *cond, pthread_mutex_t *mutex,
                                                const struct timespec *reltime)
{
    int64_t timeout_ms = 0;

    if ((cond == NULL) || (mutex == NULL)) {
        return EINVAL;
    }

    if (reltime == NULL) {
        timeout_ms = AOS_WAIT_FOREVER;
    } else {
        timeout_ms = ((reltime->tv_sec) * 1000) + ((reltime->tv_nsec) / 1000000);
        if (timeout_ms <= 0) {
            return ETIMEDOUT;
        }
    }

    return pthread_cond_timedwait_ms(cond, mutex, timeout_ms);
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    return pthread_cond_timedwait(cond, mutex, NULL);
}

int pthread_condattr_init(pthread_condattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    attr->flag = PTHREAD_DYN_INIT;
    attr->clock   = DEFAULT_COND_CLOCK;
    attr->pshared = DEFAULT_COND_SHARED;

    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    memset(attr, 0 ,sizeof(pthread_condattr_t));

    return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id)
{
    if ((attr == NULL) || (clock_id == NULL)) {
        return EINVAL;
    }

    *clock_id = attr->clock;

    return 0;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id)
{
    if (attr == NULL) {
        return EINVAL;
    }

    if ((clock_id != CLOCK_REALTIME) && (clock_id != CLOCK_MONOTONIC)) {
        return EINVAL;
    }

    attr->clock = clock_id;

    return 0;
}
