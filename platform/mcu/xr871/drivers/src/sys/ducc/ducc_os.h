/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SYS_DUCC_DUCC_OS_H_
#define _SYS_DUCC_DUCC_OS_H_

#include "kernel/os/os.h"
#include "compiler.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUCC_WAIT_FOREVER   OS_WAIT_FOREVER

/* Semaphore */
typedef OS_Semaphore_t ducc_semaphore_t;

#define ducc_semaphore_create(sem, initCount) \
	(OS_SemaphoreCreate(sem, initCount, OS_SEMAPHORE_MAX_COUNT) == OS_OK ? 0 : -1)

#define ducc_semaphore_delete(sem) \
	OS_SemaphoreDelete(sem)

#define ducc_semaphore_wait(sem) \
	(OS_SemaphoreWait(sem, OS_WAIT_FOREVER) == OS_OK ? 0 : -1)

#define ducc_semaphore_release(sem) \
	OS_SemaphoreRelease(sem)

/* Mutex */
typedef OS_Mutex_t ducc_mutex_t;

#define ducc_mutex_create(mtx)  (OS_MutexCreate(mtx) == OS_OK ? 0 : -1)
#define ducc_mutex_delete(mtx)  OS_MutexDelete(mtx)
#define ducc_mutex_lock(mtx)    OS_MutexLock(mtx, OS_WAIT_FOREVER)
#define ducc_mutex_unlock(mtx)  OS_MutexUnlock(mtx)

/* Message Queue */
typedef OS_Queue_t ducc_msgqueue_t;

#define ducc_msgqueue_create(queue, queueLen) \
	(OS_MsgQueueCreate(queue, queueLen) == OS_OK ? 0 : -1)

#define ducc_msgqueue_delete(queue) \
	OS_MsgQueueDelete(queue)

#define ducc_msgqueue_send(queue, msg, waitMS) \
	(OS_MsgQueueSend(queue, msg, waitMS) == OS_OK ? 0 : -1)

#define ducc_msgqueue_recv(queue, msg, waitMS) \
	(OS_MsgQueueReceive(queue, msg, waitMS) == OS_OK ? 0 : -1)

#define ducc_msgqueue_is_valid(queue) \
	OS_QueueIsValid(queue)

/* Thread */
typedef OS_Thread_t ducc_thread_t;
typedef OS_ThreadEntry_t ducc_thread_entry_t;

#define ducc_thread_create(thread, name, entry, arg, priority, stack_size) \
	(OS_ThreadCreate(thread, name, entry, arg, priority, stack_size) == OS_OK ? 0 : -1)

#define ducc_thread_exit(thread)        OS_ThreadDelete(thread)
#define ducc_thread_is_valid(thread)    OS_ThreadIsValid(thread)
#define ducc_msleep(msec)               OS_MSleep(msec)

/* memory */
//#define ducc_malloc(l)        malloc(l)
//#define ducc_free(p)          free(p)
#define ducc_memcpy(d, s, l)    memcpy(d, s, l)
#define ducc_memset(d, c, l)    memset(d, c, l)
#define ducc_memcmp(a, b, l)    memcmp(a, b, l)
#define ducc_memmove(d, s, n)   memmove(d, s, n)
#define ducc_strcmp(a, b)       strcmp(a, b)


#ifdef __cplusplus
}
#endif

#endif /* _SYS_DUCC_DUCC_OS_H_ */
