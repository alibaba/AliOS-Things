/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <sys/time.h>
#include <semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    kstat_t stat;

    stat = krhino_sem_dyn_create((ksem_t **)sem, "sem", value);
    if (stat == RHINO_SUCCESS) {
        return 0;
    }

    return -1;
}

sem_t *sem_open(const char *name, int oflag, ...)
{
    return 0;
}

int sem_post(sem_t *sem)
{
    kstat_t stat;

    stat = krhino_sem_give(*sem);
    if (stat == RHINO_SUCCESS) {
        return 0;
    }

    return -1;
}

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    kstat_t stat;
    tick_t ticks = RHINO_WAIT_FOREVER;
    struct timeval timeval;
    uint64_t nsec;
    int64_t sec;

    if (abs_timeout != NULL) {
        gettimeofday(&timeval, NULL);

        if ((timeval.tv_usec * 1000) > abs_timeout->tv_nsec) {
            nsec = abs_timeout->tv_nsec + 1000000000 - timeval.tv_usec * 1000;
            sec = abs_timeout->tv_sec - timeval.tv_sec - 1;
        } else {
            nsec = abs_timeout->tv_nsec - timeval.tv_usec * 1000;
            sec = abs_timeout->tv_sec - timeval.tv_sec;
        }

        if (sec < 0) {
            return -1;
        }

        ticks = sec * RHINO_CONFIG_TICKS_PER_SECOND + (nsec / 1000000)
                / (1000 / RHINO_CONFIG_TICKS_PER_SECOND);
    }

    stat = krhino_sem_take(*sem, ticks);
    if (stat == RHINO_SUCCESS) {
        return 0;
    }

    return -1;
}

int sem_trywait(sem_t *sem)
{
    kstat_t stat;

    stat = krhino_sem_take(*sem, 0);
    if (stat == RHINO_SUCCESS) {
        return 0;
    }

    return -1;
}

int sem_unlink(const char *name)
{
    return 0;
}

int sem_wait(sem_t *sem)
{
    kstat_t stat;

    stat = krhino_sem_take(*sem, RHINO_WAIT_FOREVER);
    if (stat == RHINO_SUCCESS) {
        return 0;
    }

    return -1;
}

int sem_getvalue(sem_t *sem, int *sval)
{
    sem_count_t cnt;

    krhino_sem_count_get(*sem, &cnt);
    *sval = cnt;

    return 0;
}

int sem_close(sem_t *sem)
{
    return 0;
}

int sem_destroy(sem_t *sem)
{
    kstat_t stat;

    stat = krhino_sem_dyn_del(*sem);
    if (stat == RHINO_SUCCESS) {
        return 0;
    }

    return -1;
}

