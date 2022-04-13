/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEM_FAILED ((sem_t *)0)
#define SEM_VALUE_MAX 32767

typedef struct {
    void *aos_sem;
} sem_t;

int    sem_init(sem_t *sem, int pshared, unsigned int value);
sem_t *sem_open(const char *name, int oflag, ...);
int    sem_post(sem_t *sem);
int    sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int    sem_trywait(sem_t *sem);
int    sem_unlink(const char *name);
int    sem_wait(sem_t *sem);
int    sem_getvalue(sem_t *sem, int *sval);
int    sem_close(sem_t *sem);
int    sem_destroy(sem_t *sem);

#ifdef __cplusplus
}
#endif

#endif /* _SEMAPHORE_H */
