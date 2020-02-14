/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/posix/semaphore.h"
#include "aos/posix/timer.h"

#if (POSIX_CONFIG_SEMAPHORE_ENABLE > 0)

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
    tick_t  ticks = RHINO_WAIT_FOREVER;

    if (abs_timeout != NULL) {
        if (timespec_abs_to_ticks(CLOCK_REALTIME, abs_timeout, &ticks) != 0) {
            /**
             * If you can get the semaphore immediately, you don't need to check the validity
             * of the abs_timeout and it doesn't return a timeout. So if -1 is returned, the
             * semaphore is also requested in a non-blocking manner.
             */
            ticks = 0;
        }
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

#endif
