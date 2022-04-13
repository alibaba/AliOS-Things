/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <semaphore.h>
#include <aos/kernel.h>
#include <aos/errno.h>
#include <posix/timer.h>

#define SEM_NAME_MAX 1024

/* Initialize an unnamed semaphore */
int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    int ret = 0;

    if ((sem == NULL) || (value > SEM_VALUE_MAX)) {
        errno = EINVAL;
        return -1;
    }

    /* Sharing between processes is not supported. */
    if (pshared != 0) {
        errno = ENOTSUP;
        return -1;
    }

    ret = aos_sem_new((aos_sem_t *)&(sem->aos_sem), value);
    if (ret != 0) {
        errno = -ret;
        return -1;
    }

    return 0;
}

/* Initialize and open a named semaphore */
sem_t *sem_open(const char *name, int oflag, ...)
{
    if ((name == NULL) || ((strlen(name) <= 0) || (strlen(name) > SEM_NAME_MAX))) {
        errno = EINVAL;
        return SEM_FAILED;
    }

    errno = ENOSYS;
    return SEM_FAILED;
}

/* lock a semaphore */
int sem_wait(sem_t *sem)
{
    int ret  = 0;

    if (sem == NULL) {
        errno = EINVAL;
        return -1;
    }

    ret = aos_sem_wait((aos_sem_t *)&(sem->aos_sem), AOS_WAIT_FOREVER);
    if (ret != 0) {
        errno = -ret;
        return -1;
    }

    return 0;
}

/* try to lock a semaphore */
int sem_trywait(sem_t *sem)
{
    int ret  = 0;

    if (sem == NULL) {
        errno = EINVAL;
        return -1;
    }

    ret = aos_sem_wait((aos_sem_t *)&(sem->aos_sem), 0);
    if (ret == -ETIMEDOUT) {
        errno = EAGAIN;
        return -1;
    } else if (ret < 0) {
        errno = -ret;
        return -1;
    } else {
        return 0;
    }

    return 0;
}

/* lock a semaphore */
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    int ret  = 0;
    uint64_t timeout_ms = 0;
    struct timespec cur_time = {0};
    struct timespec rel_time = {0};

    if ((sem == NULL) || (abs_timeout == NULL)) {
        errno = EINVAL;
        return -1;
    }

    if ((abs_timeout->tv_sec < 0) || (abs_timeout->tv_nsec < 0)
                                  || (abs_timeout->tv_nsec >= 1000000000UL)) {
        errno = EINVAL;
        return -1;
    }

    ret = clock_gettime(CLOCK_REALTIME, &cur_time);
    if (ret != 0) {
        return ret;
    }
    rel_time.tv_sec = abs_timeout->tv_sec - cur_time.tv_sec;
    rel_time.tv_nsec = abs_timeout->tv_nsec - cur_time.tv_nsec;
    /* Only millisecond precision is supported in AliOS Things. */
    timeout_ms = ((rel_time.tv_sec > 0) ? rel_time.tv_sec : 0) * 1000 +
                 ((rel_time.tv_nsec > 0) ? rel_time.tv_nsec : 0) / 1000000;

    ret = aos_sem_wait((aos_sem_t *)&(sem->aos_sem), timeout_ms);
    if (ret != 0) {
        errno = -ret;
        return -1;
    }

    return 0;
}

/* unlock a semaphore */
int sem_post(sem_t *sem)
{
    int ret  = 0;

    if (sem == NULL) {
        errno = EINVAL;
        return -1;
    }

    aos_sem_signal((aos_sem_t *)&(sem->aos_sem));

    return 0;
}

/* get the value of a semaphore */
int sem_getvalue(sem_t *sem, int *sval)
{
    int ret  = 0;

    if ((sem == NULL) || (sval == NULL)) {
        errno = EINVAL;
        return -1;
    }

    errno = ENOSYS;
    return -1;
}

/* close a named semaphore */
int sem_close(sem_t *sem)
{
    int ret = 0;

    if (sem == NULL) {
        errno = EINVAL;
        return -1;
    }

    errno = ENOSYS;
    return -1;
}

/* remove a named semaphore */
int sem_unlink(const char *name)
{
    int ret  = 0;

    if (name == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (strlen(name) <= 0) {
        errno = ENOENT;
        return -1;
    }

    if (strlen(name) > SEM_NAME_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    errno = ENOSYS;
    return -1;
}

/* destroy an unnamed semaphore */
int sem_destroy(sem_t *sem)
{
    if (sem == NULL) {
        errno = EINVAL;
        return -1;
    }

    aos_sem_free((aos_sem_t *)&(sem->aos_sem));

    return 0;
}
