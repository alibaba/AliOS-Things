/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

typedef unsigned long sem_t;

int sem_init(sem_t *sem, unsigned int val);

int sem_destory(sem_t *sem);

int sem_wait(sem_t *sem);

int sem_trywait(sem_t *sem);

int sem_timedwait(sem_t *sem, unsigned long ms);

int sem_post(sem_t *sem);

#endif /* SEMAPHORE_H */

