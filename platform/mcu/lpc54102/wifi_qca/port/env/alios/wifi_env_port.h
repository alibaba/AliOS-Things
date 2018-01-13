/*
 * The Clear BSD License
 * Copyright (c) 2016, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used tom  endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __WIFI_ENV_PORT_H__
#define __WIFI_ENV_PORT_H__

/*
 * This file contains definition of environment specific macros.
 * This file cannot be included directly, use #include "wifi_common.h"
 */
#include "k_api.h"

#ifndef __WIFI_PORT_H__
#error "'wifi_env_port.h' cannot be included directly"
#endif

/* TODO: find better place */
#define WLAN_NUM_OF_DEVICES 1

#ifndef MSEC_TO_TICK
#define MSEC_TO_TICK(msec) \
    ((uint32_t)(msec) * (uint32_t)RHINO_CONFIG_TICKS_PER_SECOND / 1000uL)
#endif

#ifndef TICKS_TO_MSEC
#define TICKS_TO_MSEC(tick) ((tick)*1000uL / (uint32_t)RHINO_CONFIG_TICKS_PER_SECOND)
#endif

#ifndef RXBUFFER_ACCESS_INIT
#define RXBUFFER_ACCESS_INIT(pCxt) A_MUTEX_INIT(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef RXBUFFER_ACCESS_ACQUIRE
#define RXBUFFER_ACCESS_ACQUIRE(pCxt) A_MUTEX_ACQUIRE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef RXBUFFER_ACCESS_RELEASE
#define RXBUFFER_ACCESS_RELEASE(pCxt) A_MUTEX_RELEASE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef RXBUFFER_ACCESS_DESTROY
#define RXBUFFER_ACCESS_DESTROY(pCxt) A_MUTEX_DELETE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef TXQUEUE_ACCESS_INIT
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define TXQUEUE_ACCESS_INIT(pCxt) (A_OK)
#endif

#ifndef TXQUEUE_ACCESS_ACQUIRE
#define TXQUEUE_ACCESS_ACQUIRE(pCxt) A_MUTEX_ACQUIRE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef TXQUEUE_ACCESS_RELEASE
#define TXQUEUE_ACCESS_RELEASE(pCxt) A_MUTEX_RELEASE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef TXQUEUE_ACCESS_DESTROY
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define TXQUEUE_ACCESS_DESTROY(pCxt)
#endif

#ifndef IRQEN_ACCESS_INIT
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define IRQEN_ACCESS_INIT(pCxt) (A_OK)
#endif

#ifndef IRQEN_ACCESS_ACQUIRE
#define IRQEN_ACCESS_ACQUIRE(pCxt) A_MUTEX_ACQUIRE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef IRQEN_ACCESS_RELEASE
#define IRQEN_ACCESS_RELEASE(pCxt) A_MUTEX_RELEASE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef IRQEN_ACCESS_DESTROY
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define IRQEN_ACCESS_DESTROY(pCxt)
#endif

#ifndef DRIVER_SHARED_RESOURCE_ACCESS_INIT
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define DRIVER_SHARED_RESOURCE_ACCESS_INIT(pCxt) (A_OK)
#endif

#ifndef DRIVER_SHARED_RESOURCE_ACCESS_ACQUIRE
#define DRIVER_SHARED_RESOURCE_ACCESS_ACQUIRE(pCxt) A_MUTEX_ACQUIRE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef DRIVER_SHARED_RESOURCE_ACCESS_RELEASE
#define DRIVER_SHARED_RESOURCE_ACCESS_RELEASE(pCxt) A_MUTEX_RELEASE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef DRIVER_SHARED_RESOURCE_ACCESS_DESTROY
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define DRIVER_SHARED_RESOURCE_ACCESS_DESTROY(pCxt)
#endif

#ifndef A_MUTEX_T
#define A_MUTEX_T kmutex_t
#endif

#ifndef A_MUTEX_INIT
#define A_MUTEX_INIT(mutex) a_mutex_init((mutex))
#endif

#ifndef A_MUTEX_ACQUIRE
#define A_MUTEX_ACQUIRE(mutex) a_mutex_acquire((mutex))
#endif

#ifndef A_MUTEX_RELEASE
#define A_MUTEX_RELEASE(mutex) a_mutex_release((mutex))
#endif

#ifndef A_IS_MUTEX_VALID
#define A_IS_MUTEX_VALID(mutex) a_is_mutex_valid((mutex))
#endif

#ifndef A_MUTEX_DELETE
#define A_MUTEX_DELETE(mutex) a_mutex_delete((mutex))
#endif

#ifndef A_EVENT
#define A_EVENT event_t //a wrapper for kevent_t
#endif

#ifndef A_EVENT_FLAGS
#define A_EVENT_FLAGS uint32_t
#endif

/* OSA replacement */
#ifndef A_EVENT_DELETE
#define A_EVENT_DELETE(event) a_event_delete((event))
#endif

#ifndef A_EVENT_INIT
#define A_EVENT_INIT(event, autoclear) a_event_init((event), (autoclear))
#endif

#ifndef A_EVENT_CLEAR
#define A_EVENT_CLEAR(event, flags) a_event_clear((event), (flags))
#endif

#ifndef A_EVENT_SET
#define A_EVENT_SET(event, flags) a_event_set((event), (flags))
#endif

#ifndef A_EVENT_WAIT
#define A_EVENT_WAIT(event, flagsToWait, waitAll, timeout, setFlags) \
    a_event_wait((event), (flagsToWait), (waitAll), (timeout), (setFlags))
#endif

#ifndef A_TIME_GET_MSEC
#define A_TIME_GET_MSEC a_time_get_msec
#endif

#ifndef A_ENTER_CRITICAL
#define A_ENTER_CRITICAL(mode) a_enter_critical((mode))
#endif

#ifndef A_EXIT_CRITICAL
#define A_EXIT_CRITICAL(mode) a_exit_critical((mode))
#endif

#ifndef A_CUSTOM_DRIVER_CONTEXT
#define A_CUSTOM_DRIVER_CONTEXT cust_context_t
#endif

#ifndef GET_DRIVER_CXT
#define GET_DRIVER_CXT(pCxt) ((cust_context_t *)(pCxt))
#endif

#ifndef GET_DRIVER_COMMON
#define GET_DRIVER_COMMON(pCxt) ((A_DRIVER_CONTEXT *)(((cust_context_t *)(pCxt))->pCommonCxt))
#endif

#ifndef RXBUFFER_ACCESS_INIT
#define RXBUFFER_ACCESS_INIT(pCxt) A_MUTEX_INIT(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef RXBUFFER_ACCESS_ACQUIRE
#define RXBUFFER_ACCESS_ACQUIRE(pCxt) A_MUTEX_ACQUIRE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef RXBUFFER_ACCESS_RELEASE
#define RXBUFFER_ACCESS_RELEASE(pCxt) A_MUTEX_RELEASE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef RXBUFFER_ACCESS_DESTROY
#define RXBUFFER_ACCESS_DESTROY(pCxt)
A_MUTEX_DELETE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef TXQUEUE_ACCESS_INIT
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define TXQUEUE_ACCESS_INIT(pCxt) (A_OK)
#endif

#ifndef TXQUEUE_ACCESS_ACQUIRE
#define TXQUEUE_ACCESS_ACQUIRE(pCxt) A_MUTEX_ACQUIRE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef TXQUEUE_ACCESS_RELEASE
#define TXQUEUE_ACCESS_RELEASE(pCxt) A_MUTEX_RELEASE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef TXQUEUE_ACCESS_DESTROY
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define TXQUEUE_ACCESS_DESTROY(pCxt)
#endif

#ifndef IRQEN_ACCESS_INIT
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define IRQEN_ACCESS_INIT(pCxt) (A_OK)
#endif

#ifndef IRQEN_ACCESS_ACQUIRE
#define IRQEN_ACCESS_ACQUIRE(pCxt) A_MUTEX_ACQUIRE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef IRQEN_ACCESS_RELEASE
#define IRQEN_ACCESS_RELEASE(pCxt) A_MUTEX_RELEASE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef IRQEN_ACCESS_DESTROY
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define IRQEN_ACCESS_DESTROY(pCxt)
#endif

#ifndef DRIVER_SHARED_RESOURCE_ACCESS_INIT
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define DRIVER_SHARED_RESOURCE_ACCESS_INIT(pCxt) (A_OK)
#endif

#ifndef DRIVER_SHARED_RESOURCE_ACCESS_ACQUIRE
#define DRIVER_SHARED_RESOURCE_ACCESS_ACQUIRE(pCxt) A_MUTEX_ACQUIRE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef DRIVER_SHARED_RESOURCE_ACCESS_RELEASE
#define DRIVER_SHARED_RESOURCE_ACCESS_RELEASE(pCxt) A_MUTEX_RELEASE(&(GET_DRIVER_CXT(pCxt)->utility_mutex))
#endif

#ifndef DRIVER_SHARED_RESOURCE_ACCESS_DESTROY
/* because its the same mutex as RXBUFFER_ACCESS this is NOP */
#define DRIVER_SHARED_RESOURCE_ACCESS_DESTROY(pCxt)
#endif

/*
 * A_MDELAY - used to delay specified number of milliseconds.
 */
#define A_MDELAY(msecs) a_task_delay(1 >= MSEC_TO_TICK(msecs) ? 1 : MSEC_TO_TICK(msecs))

#endif
