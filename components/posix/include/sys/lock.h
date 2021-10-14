/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

/* Support newlib internal locks */

#ifndef _SYS_LOCK_H
#define _SYS_LOCK_H

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _LOCK_T pthread_mutex_t
#define _LOCK_RECURSIVE_T pthread_mutex_t

#define __LOCK_INIT(class, lock) \
    class _LOCK_T lock = PTHREAD_MUTEX_INITIALIZER;
#define __LOCK_INIT_RECURSIVE(class, lock) __LOCK_INIT(class, lock)

#define __lock_init(_lock) pthread_mutex_init(&_lock, NULL)
#define __lock_acquire(_lock) pthread_mutex_lock(&_lock)
#define __lock_try_acquire(lock) pthread_mutex_trylock(&_lock)
#define __lock_release(_lock) pthread_mutex_unlock(&_lock)
#define __lock_close(_lock) pthread_mutex_destroy(&_lock)

#define __lock_init_recursive(_lock) pthread_mutex_init(&_lock, NULL)
#define __lock_acquire_recursive(_lock) pthread_mutex_lock(&_lock)
#define __lock_try_acquire_recursive(lock) pthread_mutex_trylock(&_lock)
#define __lock_release_recursive(_lock) pthread_mutex_unlock(&_lock)
#define __lock_close_recursive(_lock) pthread_mutex_destroy(&_lock)

#ifdef __cplusplus
}
#endif

#endif /*_SYS_LOCK_H*/
