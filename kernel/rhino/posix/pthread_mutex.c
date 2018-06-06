#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *p_mutex, const pthread_mutexattr_t *attr)
{
    kstat_t ret;

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
    kstat_t ret;

    ret = krhino_mutex_dyn_del(p_mutex->mutex);

    if (ret == RHINO_SUCCESS) {
        p_mutex->initted = PTHREAD_UNUSED_YET_OBJ;
        return 0;
    }

    return -1;
}

int pthread_mutex_lock(pthread_mutex_t *p_mutex)
{
    kstat_t ret;

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
    kstat_t ret;

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
    kstat_t ret;

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
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    return 0;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int  pshared)
{
    return 0;
}

int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared)
{
    return 0;
}

