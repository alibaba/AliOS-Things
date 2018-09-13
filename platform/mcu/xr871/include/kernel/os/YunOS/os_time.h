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
#ifndef _KERNEL_OS_YUNOS_OS_TIME_H_
#define _KERNEL_OS_YUNOS_OS_TIME_H_

#include "kernel/os/YunOS/os_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Parameters of time */
#define OS_MSEC_PER_SEC		1000UL
#define OS_USEC_PER_MSEC	1000UL
#define OS_USEC_PER_SEC		1000000UL

/* system clock's frequency, ticks per second */
#define OS_HZ				RHINO_CONFIG_TICKS_PER_SECOND

/* usec per tick (1000000 / OS_HZ) */
#define OS_TICK				(OS_USEC_PER_SEC / OS_HZ)

/* The number of ticks since boot. */
#define OS_GetTicks()		((uint32_t)(krhino_sys_tick_get() / OS_MSEC_PER_SEC))
	/* Due to portTICK_TYPE_IS_ATOMIC is 1, call xTaskGetTickCount() in ISR is safe also */

/* The number of seconds since boot. */
#define OS_GetTime()		(krhino_sys_time_get() / OS_HZ)

/* Time conversion */
#define OS_SecsToTicks(sec)		((uint32_t)(sec) * OS_HZ)
#define	OS_MSecsToTicks(msec)	((uint32_t)(krhino_ms_to_ticks(msec)))
#define	OS_TicksToMSecs(t)		((uint32_t)(krhino_ticks_to_ms(t)))
#define	OS_TicksToSecs(t)		((uint32_t)((krhino_ticks_to_ms(t)) / OS_MSEC_PER_SEC))

#define OS_GetJiffies()			OS_GetTicks()
#define OS_SecsToJiffies(sec)	OS_SecsToTicks(sec)
#define	OS_MSecsToJiffies(msec)	OS_MSecsToTicks(msec)
#define	OS_JiffiesToMSecs(j)	OS_TicksToMSecs(j)
#define	OS_JiffiesToSecs(j)		OS_TicksToSecs(j)

/* sleep */
#define OS_MSleep(msec)			krhino_task_sleep(krhino_ms_to_ticks(msec))
#define OS_Sleep(sec)			OS_MSleep((sec) * OS_MSEC_PER_SEC)
#define OS_SSleep(sec)			OS_Sleep(sec)

#define OS_TimeAfter(a, b)              ((int32_t)(b) - (int32_t)(a) < 0)
#define OS_TimeBefore(a, b)             OS_TimeAfter(b, a)
#define OS_TimeAfterEqual(a, b)         ((int32_t)(a) - (int32_t)(b) >= 0)
#define OS_TimeBeforeEqual(a, b)        OS_TimeAfterEqual(b, a)

/* rand, read value from Cortex-M SYST_CVR register */
#define OS_Rand32()                                                 \
	((uint32_t)(((*((volatile uint32_t *)0xE000E018)) & 0xffffff) | \
	            (OS_GetTicks() << 24)))

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_YUNOS_OS_TIME_H_ */
