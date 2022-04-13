/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <k_api.h>
#include <pthread.h>
#include <timer.h>
#include <sys/time.h>

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    int ret = -1;

    if (cond == NULL) {
        return EINVAL;
    }

    memset(cond, 0, sizeof(pthread_cond_t));
    ret = krhino_mutex_dyn_create((kmutex_t **)&cond->lock, "mutex");
    if (ret != 0) {
        return EAGAIN;
    }

    ret = krhino_sem_dyn_create((ksem_t **)&cond->wait_sem, "sem", 0);
    if (ret != 0) {
        krhino_mutex_dyn_del(cond->lock);
        cond->lock = NULL;
        return EAGAIN;
    }

    ret = krhino_sem_dyn_create((ksem_t **)&cond->wait_done, "sem", 0);
    if (ret != 0) {
        krhino_mutex_dyn_del(cond->lock);
        cond->lock = NULL;
        krhino_sem_dyn_del(cond->wait_sem);
        cond->wait_sem = NULL;
        return EAGAIN;
    }

    cond->waiting = cond->signals = 0;

    if (attr != NULL) {
        cond->attr = *attr;
    }

    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    if (cond != NULL) {
        if (cond->lock != NULL) {
            krhino_mutex_dyn_del(cond->lock);
            cond->lock = NULL;
        }

        if (cond->wait_sem != NULL) {
            krhino_sem_dyn_del(cond->wait_sem);
            cond->wait_sem = NULL;
        }

        if (cond->wait_done != NULL) {
            krhino_sem_dyn_del(cond->wait_done);
            cond->wait_done = NULL;
        }
    }

    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    int i           = 0;
    int num_waiting = 0;

    /* If there are waiting threads not already signalled, then
    signal the condition and wait for the thread to respond.
    */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);

    if (cond->waiting > cond->signals) {
        num_waiting = (cond->waiting - cond->signals);
        cond->signals = cond->waiting;

        for (i = 0; i < num_waiting; ++i) {
            krhino_sem_give(cond->wait_sem);
        }
        /* Now all released threads are blocked here, waiting for us.
        Collect them all (and win fabulous prizes!) :-)
        */
        krhino_mutex_unlock(cond->lock);
        for (i = 0; i < num_waiting; ++i) {
            krhino_sem_take(cond->wait_done, RHINO_WAIT_FOREVER);
        }
    } else {
        krhino_mutex_unlock(cond->lock);
    }

    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    /* If there are waiting threads not already signalled, then
    signal the condition and wait for the thread to respond.
    */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);
    if (cond->waiting > cond->signals) {
        (cond->signals)++;
        krhino_sem_give(cond->wait_sem);
        krhino_mutex_unlock(cond->lock);
        krhino_sem_take(cond->wait_done, RHINO_WAIT_FOREVER);
    } else {
        krhino_mutex_unlock(cond->lock);
    }

    return 0;
}

static int pthread_cond_timedwait_tick(pthread_cond_t *cond, pthread_mutex_t *mutex,
                                       const tick_t ticks)
{
    int ret = 0;

    /* Obtain the protection mutex, and increment the number of waiters.
       This allows the signal mechanism to only perform a signal if there
       are waiting threads.
     */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);
    (cond->waiting)++;
    krhino_mutex_unlock(cond->lock);

    /* Unlock the mutex, as is required by condition variable semantics */
    krhino_mutex_unlock(mutex->mutex);

    /* Wait for a signal */
    ret = krhino_sem_take(cond->wait_sem, ticks);

    /* Let the signaler know we have completed the wait, otherwise
           the signaler can race ahead and get the condition semaphore
           if we are stopped between the mutex unlock and semaphore wait,
           giving a deadlock.  See the following URL for details:
        http://www-classic.be.com/aboutbe/benewsletter/volume_III/Issue40.html
    */
    krhino_mutex_lock(cond->lock, RHINO_WAIT_FOREVER);

    if (ret == RHINO_BLK_TIMEOUT) {
        ret = ETIMEDOUT;
    }

    if (cond->signals > 0) {
        /* If we timed out, we need to eat a condition signal */
        if (ret == ETIMEDOUT) {
            krhino_sem_take(cond->wait_sem, RHINO_WAIT_FOREVER);
        }
        /* We always notify the signal thread that we are done */
        krhino_sem_give(cond->wait_done);

        /* Signal handshake complete */
        (cond->signals)--;
    }

    (cond->waiting)--;

    krhino_mutex_unlock(cond->lock);

    /* Lock the mutex, as is required by condition variable semantics */
    krhino_mutex_lock(mutex->mutex, RHINO_WAIT_FOREVER);

    return ret;
}


int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
    tick_t ticks = 0;
    struct timeval now_tv = {0};
    struct timespec now_tp = {0};
    int64_t timeout_ms = 0;

    if ((cond == NULL) || (mutex == NULL)) {
        return EINVAL;
    }

    if (abstime == NULL) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        if (cond->attr.clock == CLOCK_MONOTONIC) {
            clock_gettime(CLOCK_MONOTONIC, &now_tp);
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
        ticks = krhino_ms_to_ticks((sys_time_t)timeout_ms);
    }

    return pthread_cond_timedwait_tick(cond, mutex, ticks);
}

int pthread_cond_timedwait_relative_np(pthread_cond_t *cond, pthread_mutex_t *mutex,
                                       const struct timespec *reltime)
{
    tick_t ticks = 0;
    int64_t timeout_ms = 0;

    if ((cond == NULL) || (mutex == NULL)) {
        return EINVAL;
    }

    if (reltime == NULL) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        timeout_ms = ((reltime->tv_sec) * 1000) + ((reltime->tv_nsec) / 1000000);
        if (timeout_ms <= 0) {
            return ETIMEDOUT;
        }
        ticks = krhino_ms_to_ticks((sys_time_t)timeout_ms);
    }

    return pthread_cond_timedwait_tick(cond, mutex, ticks);
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

    attr->is_initialized = 1;

    attr->clock   = DEFAULT_COND_CLOCK;
    attr->pshared = DEFAULT_COND_SHARED;

    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    memset(attr, 0, sizeof(pthread_condattr_t));

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

