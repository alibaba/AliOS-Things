/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxLock.h
 * Description : Lock
 * History :
 *
 */

#ifndef CDX_LOCK_H
#define CDX_LOCK_H
#include <pthread.h>

typedef pthread_mutex_t CdxMutexT;
typedef pthread_cond_t CdxCondT;

#define CdxMutexInit(mutex) pthread_mutex_init(mutex, NULL)
#define CdxMutexDestroy(mutex) pthread_mutex_destroy(mutex)
#define CdxMutexLock(mutex) pthread_mutex_lock(mutex)
#define CdxMutexTrylock(mutex)pthread_mutex_trylock(mutex)
#define CdxMutexUnlock(mutex) pthread_mutex_unlock(mutex)

#define CdxCondInit(cond) pthread_cond_init(cond, NULL)
#define CdxCondDestroy(cond) pthread_cond_destroy(cond)
#define CdxCondWait(cond, mutex) pthread_cond_wait(cond, mutex)
#define CdxCondTimedwait(cond, mutex, time) \
                            pthread_cond_timedwait(cond, mutex, time)
#define CdxCondSignal(cond) pthread_cond_signal(cond)
#define CdxCondBroadcast(cond) pthread_cond_broadcast(cond)

#endif
