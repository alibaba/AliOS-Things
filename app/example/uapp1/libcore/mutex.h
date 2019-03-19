/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef MUTEX_H
#define MUTEX_H

typedef unsigned long mutex_t;

int mutex_init(mutex_t *mutex);

int mutex_lock(mutex_t *mutex);

int mutex_timedlock(mutex_t *mutex, unsigned long ms);

int mutex_trylock(mutex_t *mutex);

int mutex_unlock(mutex_t *mutex);

int mutex_destory(mutex_t *mutex);

#endif /* MUTEX */
