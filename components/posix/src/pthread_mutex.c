/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <aos/kernel.h>

#include "internal/pthread.h"


#define PTHREAD_MUTEXATTR_IS_INITED(x) do { if ((x)->flag != PTHREAD_DYN_INIT) return EINVAL; } \
                                       while (0)


int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    int ret = 0;

    if (mutex == NULL) {
        return EINVAL;
    }
    memset(mutex, 0, sizeof(pthread_mutex_t));

    if (attr != NULL) {
        mutex->attr = *attr;
    }

    ret = aos_mutex_new((aos_mutex_t*)(&(mutex->mutex)));
    if (ret != 0) {
        return -1;
    }

    mutex->flag = PTHREAD_DYN_INIT;
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    if (mutex == NULL) {
        return EINVAL;
    }

    if (mutex->flag == PTHREAD_STATIC_INIT) {
        memset(mutex, 0, sizeof(pthread_mutex_t));
        return 0;
    } else if (mutex->flag == PTHREAD_DYN_INIT) {
        aos_mutex_free((aos_mutex_t*)(&(mutex->mutex)));
        memset(mutex, 0, sizeof(pthread_mutex_t));
        return 0;
    } else {
        return EINVAL;
    }
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    int ret = 0;

    if (mutex == NULL) {
        return EINVAL;
    }

    /* The mutex is initted by PTHREAD_MUTEX_INITIALIZER */
    if (mutex->flag == PTHREAD_STATIC_INIT) {
        ret = pthread_mutex_init(mutex, NULL);
        if (ret != 0) {
            return -1;
        }
    }

    ret = aos_mutex_lock((aos_mutex_t*)(&(mutex->mutex)), AOS_WAIT_FOREVER);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    int ret = 0;

    if (mutex == NULL) {
        return EINVAL;
    }

    if (mutex->flag != PTHREAD_DYN_INIT) {
        return -1;
    }

    ret = aos_mutex_unlock((aos_mutex_t*)(&(mutex->mutex)));
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    int ret = 0;

    if (mutex == NULL) {
        return EINVAL;
    }

    /* The mutex is initted by PTHREAD_MUTEX_INITIALIZER */
    if (mutex->flag == PTHREAD_STATIC_INIT) {
        ret = pthread_mutex_init(mutex, NULL);
        if (ret != 0) {
            return -1;
        }
    }

    ret = aos_mutex_lock((aos_mutex_t*)(&(mutex->mutex)), AOS_NO_WAIT);
    if (ret == -ETIMEDOUT) {
        return EBUSY;
    } else if (ret != 0) {
        return -1;
    } else {
        return 0;
    }
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *at)
{
    int ret = 0;

    if (mutex == NULL) {
        return EINVAL;
    }

    /* The mutex is initted by PTHREAD_MUTEX_INITIALIZER */
    if (mutex->flag == PTHREAD_STATIC_INIT) {
        ret = pthread_mutex_init(mutex, NULL);
        if (ret != 0) {
            return -1;
        }
    }

    ret = pthread_mutex_trylock(mutex);
    if (ret != EBUSY) {
        return ret;
    }

    unsigned int timeout = at->tv_sec * 1000 +  at->tv_nsec / 1000;
    ret = aos_mutex_lock((aos_mutex_t*)(&(mutex->mutex)), timeout);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int pthread_mutex_getprioceiling(const pthread_mutex_t *mutex, int *prioceiling)
{
    if ((mutex == NULL) || (prioceiling == NULL)) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(&(mutex->attr));

    *prioceiling = mutex->attr.prioceiling;

    return 0;
}

int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling,
                                 int *old_ceiling)
{
    if (mutex == NULL) {
        return EINVAL;
    }

    if ((prioceiling < sched_get_priority_min(SCHED_FIFO)) ||
        (prioceiling > sched_get_priority_max(SCHED_FIFO))) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(&(mutex->attr));

    if (old_ceiling != NULL) {
        *old_ceiling = mutex->attr.prioceiling;
    }

    mutex->attr.prioceiling = prioceiling;

    return 0;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    attr->flag = PTHREAD_DYN_INIT;
    attr->type        = PTHREAD_MUTEX_DEFAULT;
    attr->protocol    = DEFAULT_MUTEX_PROCOCOL;
    attr->prioceiling = DEFAULT_MUTEX_PRIOCEILING;
    attr->pshared     = DEFAULT_MUTEX_PSHARED;

    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    memset(attr, 0, sizeof(pthread_mutexattr_t));

    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
    if ((attr == NULL) || (type == NULL)) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(attr);

    *type = attr->type;

    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    if (attr == NULL) {
        return EINVAL;
    }

    if ((type < PTHREAD_MUTEX_NORMAL) || (type > PTHREAD_MUTEX_ERRORCHECK)) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(attr);

    attr->type = type;

    return 0;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol)
{
    if ((attr == NULL) || (protocol == NULL)) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(attr);

    *protocol = attr->protocol;

    return 0;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol)
{
    if (attr == NULL) {
        return EINVAL;
    }

    if ((protocol < PTHREAD_PRIO_NONE) || (protocol > PTHREAD_PRIO_PROTECT)) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(attr);

    attr->protocol = protocol;

    return 0;
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr,
                                     int *prioceiling)
{
    if ((attr == NULL) || (prioceiling == NULL)) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(attr);

    *prioceiling = attr->prioceiling;

    return 0;
}

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling)
{
    if (attr == NULL) {
        return EINVAL;
    }

    if ((prioceiling < sched_get_priority_min(SCHED_FIFO)) ||
        (prioceiling > sched_get_priority_max(SCHED_FIFO))) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(attr);

    attr->prioceiling = prioceiling;

    return 0;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared)
{
    if (attr == NULL) {
        return EINVAL;
    }

    if ((pshared < PTHREAD_PROCESS_PRIVATE) || (pshared > PTHREAD_PROCESS_SHARED)) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(attr);

    attr->pshared = pshared;

    return 0;
}

int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared)
{
    if ((attr == NULL) || (pshared == NULL)) {
        return EINVAL;
    }

    PTHREAD_MUTEXATTR_IS_INITED(attr);

    *pshared = attr->pshared;

    return 0;
}
