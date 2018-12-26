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
#ifndef _KERNEL_OS_YUNOS_OS_SEMAPHORE_H_
#define _KERNEL_OS_YUNOS_OS_SEMAPHORE_H_

#include "kernel/os/YunOS/os_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OS_Semaphore {
	ksem_t sem;
	uint32_t count;
	uint32_t max_count;
} OS_Semaphore_t;

static __inline OS_Status OS_SemaphoreCreate(OS_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount)
{
	if (RHINO_SUCCESS == krhino_sem_create(&sem->sem, "UNDEF", (sem_count_t)initCount)) {
		sem->count = initCount;
		sem->max_count = maxCount;
		return OS_OK;
	} else {
		return OS_FAIL;
	}
}

static __inline OS_Status OS_SemaphoreCreateBinary(OS_Semaphore_t *sem)
{
	if (RHINO_SUCCESS == krhino_sem_create(&sem->sem, "UNDEF", 0)) {
		sem->count = 0;
		sem->max_count = 1;
		return OS_OK;
	} else {
		return OS_FAIL;
	}
}

static __inline OS_Status OS_SemaphoreDelete(OS_Semaphore_t *sem)
{
	if (RHINO_SUCCESS == krhino_sem_del(&sem->sem)) {
		return OS_OK;
	} else {
		return OS_FAIL;
	}
}

static __inline OS_Status OS_SemaphoreWait(OS_Semaphore_t *sem, OS_Time_t waitMS)
{
	tick_t ticks = waitMS == OS_WAIT_FOREVER ? OS_WAIT_FOREVER : OS_MSecsToTicks(waitMS);
	OS_Status ret;
	kstat_t sta;
	uint32_t time_tick = OS_GetTicks();

	if (sem == NULL) {
		return OS_FAIL;
	}

	sta = krhino_sem_take(&sem->sem, ticks);
	if(sta == RHINO_SUCCESS) {
		if (sem->count)
			sem->count--;
		ret = OS_OK;
	} else {
		ret = OS_E_TIMEOUT;
	}

	return ret;
}

static __inline OS_Status OS_SemaphoreRelease(OS_Semaphore_t *sem)
{
	if (sem->max_count == 1 && sem->count == 1) {
		return OS_FAIL;
	}
	if (RHINO_SUCCESS == krhino_sem_give(&sem->sem)) {
		if (sem->count < sem->max_count)
			sem->count++;
		return OS_OK;
	} else {
		return OS_FAIL;
	}
}

static __inline int OS_SemaphoreIsValid(OS_Semaphore_t *sem)
{
	return (sem->sem.blk_obj.obj_type == RHINO_SEM_OBJ_TYPE);
}

static __inline void OS_SemaphoreSetInvalid(OS_Semaphore_t *sem)
{
	sem->sem.blk_obj.obj_type = RHINO_OBJ_TYPE_NONE;
}

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_YUNOS_OS_SEMAPHORE_H_ */
