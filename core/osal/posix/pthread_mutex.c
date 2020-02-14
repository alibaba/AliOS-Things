/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/posix/pthread.h"

#if (POSIX_CONFIG_PTHREAD_ENABLE > 0)

int pthread_mutex_init(pthread_mutex_t *p_mutex, const pthread_mutexattr_t *attr)
{
    kstat_t ret = -1;

    if (attr != NULL) {
        p_mutex->attr = *attr;
    } else {
        pthread_mutexattr_init(&p_mutex->attr);
    }

    ret = krhino_mutex_dyn_create(&p_mutex->mutex, "mutex");
    if (ret == RHINO_SUCCESS) {
        p_mutex->initted = PTHREAD_INITIALIZED_OBJ;
        return 0;
    } else {
        return -1;
    }
}

int pthread_mutex_destroy(pthread_mutex_t *p_mutex)
{
    kstat_t ret = -1;

    ret = krhino_mutex_dyn_del(p_mutex->mutex);
    if (ret == RHINO_SUCCESS) {
        p_mutex->initted = PTHREAD_UNUSED_YET_OBJ;

        return 0;
    }

    return -1;
}

int pthread_mutex_lock(pthread_mutex_t *p_mutex)
{
    kstat_t ret = -1;

    if (p_mutex->initted != PTHREAD_INITIALIZED_OBJ) {
        return -1;
    }

    ret = krhino_mutex_lock(p_mutex->mutex, RHINO_WAIT_FOREVER);
    if ((ret == RHINO_SUCCESS) || (ret == RHINO_MUTEX_OWNER_NESTED)) {
        return 0;
    }

    return -1;
}

int pthread_mutex_unlock(pthread_mutex_t *p_mutex)
{
    kstat_t ret = -1;

    if (p_mutex->initted != PTHREAD_INITIALIZED_OBJ) {
        return -1;
    }

    ret = krhino_mutex_unlock(p_mutex->mutex);
    if ((ret == RHINO_SUCCESS) || (ret == RHINO_MUTEX_OWNER_NESTED)) {
        return 0;
    }

    return -1;
}

int pthread_mutex_trylock(pthread_mutex_t *p_mutex)
{
    kstat_t ret = -1;

    if (p_mutex->initted != PTHREAD_INITIALIZED_OBJ) {
        return -1;
    }

    ret = krhino_mutex_lock(p_mutex->mutex, 0);
    if ((ret == RHINO_SUCCESS) || (ret == RHINO_MUTEX_OWNER_NESTED)) {
        return 0;
    }

    return -1;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    if (attr == NULL) {
        return -1;
    }

    attr->is_initialized = 1;

    attr->type        = DEFAULT_MUTEX_TYPE;
    attr->protocol    = DEFAULT_MUTEX_PROCOCOL;
    attr->prioceiling = DEFAULT_MUTEX_PRIOCEILING;
    attr->pshared     = DEFAULT_MUTEX_PSHARED;

    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    if (attr == NULL) {
        return -1;
    }

    memset(attr, 0 ,sizeof(pthread_mutexattr_t));

    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
    if ((attr == NULL)||(type == NULL)) {
        return -1;
    }

    *type = attr->type;

    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    if (attr == NULL) {
        return -1;
    }

    /* kernel only support PTHREAD_MUTEX_RECURSIVE mode */
    if (type == PTHREAD_MUTEX_RECURSIVE) {
        attr->type = type;
    } else {
        return -1;
    }

    return 0;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *restrict attr, int *restrict protocol)
{
    if ((attr == NULL)||(protocol == NULL)) {
        return -1;
    }

    *protocol = attr->protocol;

    return 0;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol)
{
    if (attr == NULL) {
        return -1;
    }

    /* kernel only support PTHREAD_PRIO_INHERIT mode */
    if (protocol == PTHREAD_PRIO_INHERIT) {
        attr->protocol = protocol;
    } else {
        return -1;
    }

    return 0;
}

int pthread_mutex_getprioceiling(const pthread_mutex_t *restrict mutex, int *restrict prioceiling)
{
    if ((mutex == NULL) || (prioceiling == NULL)) {
        return -1;
    }

    *prioceiling = mutex->attr.prioceiling;

    return 0;
}

int pthread_mutex_setprioceiling(pthread_mutex_t *restrict mutex, int prioceiling, int *restrict old_ceiling)
{
    if (mutex == NULL) {
        return -1;
    }

    if (old_ceiling != NULL) {
        *old_ceiling = mutex->attr.prioceiling;
    }

    /* the hightest prio of mutex is equal to POSIX_HIGH_PRI, can not be modified */
    mutex->attr.prioceiling = POSIX_HIGH_PRI;

    return 0;
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *restrict attr, int *restrict prioceiling)
{
    if ((attr == NULL)||(prioceiling == NULL)) {
        return -1;
    }

    *prioceiling = attr->prioceiling;

    return 0;
}

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling)
{
    if (attr == NULL) {
        return -1;
    }

    /* the hightest prio of mutex is equal to POSIX_HIGH_PRI, can not be modified */
    attr->prioceiling = POSIX_HIGH_PRI;

    return 0;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared)
{
    if (attr == NULL) {
        return -1;
    }

    /* since there is no implementation of process, the pshared must
       be PTHREAD_PROCESS_PRIVATE */
    if (pshared == PTHREAD_PROCESS_PRIVATE) {
        attr->pshared = pshared;
    } else {
        return -1;
    }

    return 0;
}

int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared)
{
    if ((attr == NULL)||(pshared == NULL)) {
        return -1;
    }

    *pshared = attr->pshared;

    return 0;
}

#endif
