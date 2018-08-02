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
#define HAL_DisableIRQ()	arch_irq_disable()
#define HAL_EnableIRQ()		arch_irq_enable()

/* Check if in ISR context or not */
#define HAL_IsISRContext()  __get_IPSR()

/* Critical Sections */
#define HAL_EnterCriticalSection()		arch_irq_save()
#define HAL_ExitCriticalSection(flags)	arch_irq_restore(flags)

/* Semaphore */
typedef OS_Semaphore_t HAL_Semaphore;

#define HAL_SemaphoreInit(sem, initCount, maxCount) \
	(OS_SemaphoreCreate(sem, initCount, maxCount) == OS_OK ? HAL_OK : HAL_ERROR)

#define HAL_SemaphoreInitBinary(sem) \
	(OS_SemaphoreCreateBinary(sem) == OS_OK ? HAL_OK : HAL_ERROR)

#define HAL_SemaphoreDeinit(sem) \
	(OS_SemaphoreDelete(sem) == OS_OK ? HAL_OK : HAL_ERROR)

#define HAL_SemaphoreWait(sem, msec) \
	(OS_SemaphoreWait(sem, msec) == OS_OK ? HAL_OK : HAL_ERROR)

#define HAL_SemaphoreRelease(sem) \
	(OS_SemaphoreRelease(sem) == OS_OK ? HAL_OK : HAL_ERROR)

/* Mutex */
typedef OS_Mutex_t HAL_Mutex;

#define HAL_MutexInit(mtx) \
	(OS_MutexCreate(mtx) == OS_OK ? HAL_OK : HAL_ERROR)

#define HAL_MutexDeinit(mtx) \
	(OS_MutexDelete(mtx) == OS_OK ? HAL_OK : HAL_ERROR)

#define HAL_MutexLock(mtx, msec) \
	(OS_MutexLock(mtx, msec) == OS_OK ? HAL_OK : HAL_ERROR)

#define HAL_MutexUnlock(mtx) \
	(OS_MutexUnlock(mtx) == OS_OK ? HAL_OK : HAL_ERROR)

/* time */
#define HAL_Ticks() 			OS_GetTicks()
#define HAL_MSleep(msec)		OS_MSleep(msec)

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
