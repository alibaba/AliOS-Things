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

#ifndef _DRIVER_CHIP_HAL_OS_H_
#define _DRIVER_CHIP_HAL_OS_H_

#include "kernel/os/os.h"
#include "sys/interrupt.h"
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IRQ disable/enable */
__STATIC_INLINE void HAL_DisableIRQ(void)
{
	arch_irq_disable();
}

__STATIC_INLINE void HAL_EnableIRQ(void)
{
	arch_irq_enable();
}

/* Critical Sections */
__STATIC_INLINE unsigned long HAL_EnterCriticalSection(void)
{
	return arch_irq_save(); // temp implementation
}

__STATIC_INLINE void HAL_ExitCriticalSection(unsigned long flags)
{
	arch_irq_restore(flags); // temp implementation
}

/* Semaphore */
typedef OS_Semaphore_t HAL_Semaphore;

__STATIC_INLINE HAL_Status HAL_SemaphoreInit(HAL_Semaphore *sem, uint32_t initCount, uint32_t maxCount)
{
	return OS_SemaphoreCreate(sem, initCount, maxCount) == OS_OK ? HAL_OK : HAL_ERROR;
}

__STATIC_INLINE HAL_Status HAL_SemaphoreInitBinary(HAL_Semaphore *sem)
{
	return OS_SemaphoreCreateBinary(sem) == OS_OK ? HAL_OK : HAL_ERROR;
}

__STATIC_INLINE HAL_Status HAL_SemaphoreDeinit(HAL_Semaphore *sem)
{
	return OS_SemaphoreDelete(sem) == OS_OK ? HAL_OK : HAL_ERROR;
}

__STATIC_INLINE HAL_Status HAL_SemaphoreWait(HAL_Semaphore *sem, uint32_t msec)
{
	return OS_SemaphoreWait(sem, msec) == OS_OK ? HAL_OK : HAL_ERROR;
}

__STATIC_INLINE HAL_Status HAL_SemaphoreRelease(HAL_Semaphore *sem)
{
	return OS_SemaphoreRelease(sem) == OS_OK ? HAL_OK : HAL_ERROR;
}

/* Mutex */
typedef OS_Mutex_t HAL_Mutex;

__STATIC_INLINE HAL_Status HAL_MutexInit(HAL_Mutex *mtx)
{
	return OS_MutexCreate(mtx) == OS_OK ? HAL_OK : HAL_ERROR;
}

__STATIC_INLINE HAL_Status HAL_MutexDeinit(HAL_Mutex *mtx)
{
	return OS_MutexDelete(mtx) == OS_OK ? HAL_OK : HAL_ERROR;
}

__STATIC_INLINE HAL_Status HAL_MutexLock(HAL_Mutex *mtx, uint32_t msec)
{
	return OS_MutexLock(mtx, msec) == OS_OK ? HAL_OK : HAL_ERROR;
}

__STATIC_INLINE HAL_Status HAL_MutexUnlock(HAL_Mutex *mtx)
{
	return OS_MutexUnlock(mtx) == OS_OK ? HAL_OK : HAL_ERROR;
}

/* time */
__STATIC_INLINE uint32_t HAL_Ticks(void)
{
	return OS_GetTicks();
}

__STATIC_INLINE void HAL_MSleep(uint32_t msec)
{
	OS_MSleep(msec);
}

#define HAL_SecsToTicks(sec)	OS_SecsToTicks(sec)
#define HAL_MSecsToTicks(msec)	OS_MSecsToTicks(msec)
#define HAL_TicksToMSecs(t)		OS_TicksToMSecs(t)
#define HAL_TicksToSecs(t)		OS_TicksToSecs(t)

#define HAL_TimeAfter(a, b) 		OS_TimeAfter(a, b)
#define HAL_TimeBefore(a, b)		OS_TimeAfter(b, a)
#define HAL_TimeAfterEqual(a, b)	OS_TimeAfterEqual(a, b)
#define HAL_TimeBeforeEqual(a, b)	OS_TimeAfterEqual(b, a)

/* memory */
#define HAL_Malloc(l)			malloc(l)
#define HAL_Free(p)				free(p)
#define HAL_Memcpy(d, s, l)		memcpy(d, s, l)
#define HAL_Memset(d, c, l)		memset(d, c, l)
#define HAL_Memcmp(a, b, l)		memcmp(a, b, l)
#define HAL_Memmove(d, s, n)	memmove(d, s, n)

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_OS_H_ */
