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

#ifndef __WIFI_ENV_H__
#define __WIFI_ENV_H__

#include <stdint.h>

/* contains only prototypes and struct. no defines !!! */
#include "wifi_common.h"

// TODO: remove
#include "a_config.h"

/*! @brief The event flags are cleared automatically or manually.*/
typedef enum _osa_event_clear_mode_t
{
    kEventAutoClear = 0U,  /*!< The flags of the event will be cleared automatically. */
    kEventManualClear = 1U /*!< The flags of the event will be cleared manually.      */
} osa_event_clear_mode_t;

/*! @brief Locks the task scheduler or disables interrupt in critical section. */
typedef enum _osa_critical_section_mode_t
{
    kCriticalLockSched = 0U, /*!< Lock scheduler in critical section.      */
    kCriticalDisableInt = 1U /*!< Disable interrupt in critical selection. */
} osa_critical_section_mode_t;

/*! @brief Type for an event group object in FreeRTOS OS */
// typedef EventGroupHandle_t event_freertos;

/*! @brief Type for an event group object */

typedef struct alios_event_t
{
    kevent_t event;  /*!< FreeRTOS OS event handler       */
    osa_event_clear_mode_t clearMode; /*!< Auto clear or manual clear   */
} event_t;

typedef struct
{
    uint32_t dummy;
} cust_hw_context_t;

typedef struct
{
    cust_hw_context_t *hw_context;

    /* PORT_NOTE: utility_mutex is a mutex used to protect resources that
     *  might be accessed by multiple task/threads in a system.
     *  It is used by the ACQUIRE/RELEASE macros below.  If the target
     *  system is single threaded then this mutex can be omitted and
     *  the macros should be blank.*/
    A_MUTEX_T utility_mutex;
    /* PORT_NOTE: connectStateCB allows an app to get feedback/notification
     *  as the WIFI connection state changes. If used it should be populated
     *  with a function pointer by an IOCTL or system equivalent. */
    void *connectStateCB;
    /* PORT_NOTE: pScanOut stores scan results in a buffer so that they
     *  can be retrieved aysnchronously by a User task. */
    // uint8_t*      pScanOut; /* callers buffer to hold results. */
    // uint16_t      scanOutSize;/* size of pScanOut buffer */
    // uint16_t      scanOutCount;/* current fill count of pScanOut buffer */

    /* PORT_NOTE: These 2 events are used to block & wake their respective
     *  tasks. */
    A_EVENT driverWakeEvent;              /* wakes the driver thread */
    A_EVENT userWakeEvent;                /* wakes blocked user threads */
                                          //#if T_SELECT_VER1
    A_EVENT sockSelectWakeEvent;          /* wakes t_select  */
                                          //#endif //T_SELECT_VER1
                                          //#endif
                                          /* PORT_NOTE: These 2 elements provide pointers to system context.
                                           *  The pDriverParam is intended provide definition for info such as
                                           *  which GPIO's to use for the SPI bus.  The pUpperCxt should be
                                           *  populated with a system specific object that is required when the
                                           *  driver calls any system API's. In case of multi interface, the
                                           *  different enet pointers are stored in the corresponding array index
                                           */
    void *pDriverParam;                   /* param struct containing initialization params */
    void *pUpperCxt[WLAN_NUM_OF_DEVICES]; /* back pointer to the MQX enet object. */
                                          /* PORT_NOTE: pCommonCxt stores the drivers common context. It is
                                           *  accessed by the common source via GET_DRIVER_COMMON() below. */
    void *pCommonCxt;                     /* the common driver context link */
    boolean driverShutdown;               /* used to shutdown the driver */
    void *promiscuous_cb;                 /* used to feed rx frames in promiscuous mode. */
                                          //#if ENABLE_P2P_MODE
    void *p2pevtCB;
    boolean p2pEvtState;
    boolean p2pevtflag;
    //#if MANUFACTURING_SUPPORT
    uint32_t testCmdRespBufLen;
    //#endif
    //    A_CUSTOM_HCD_CXT    *customhcdcxt;
    void *httpPostCb; /*callback handler for http post events*/
    void *httpPostCbCxt;
    void *otaCB; /* callback handler for OTA event */
    void *probeReqCB;

} cust_context_t;

/*TODO: check return value */
void a_free(void *addr, uint8_t id);
void *a_malloc(int32_t size, uint8_t id);
A_STATUS a_mutex_init(A_MUTEX_T *pMutex);
A_STATUS a_mutex_acquire(A_MUTEX_T *pMutex);
A_STATUS a_mutex_release(A_MUTEX_T *pMutex);
boolean a_is_mutex_valid(A_MUTEX_T *pMutex);
A_STATUS a_mutex_delete(A_MUTEX_T *pMutex);
A_STATUS a_event_delete(A_EVENT *pEvent);
A_STATUS a_event_init(A_EVENT *pEvent, osa_event_clear_mode_t clearMode);
A_STATUS a_event_clear(A_EVENT *pEvent, A_EVENT_FLAGS flagsToClear);
A_STATUS a_event_set(A_EVENT *pEvent, A_EVENT_FLAGS flagsToSet);
A_STATUS a_event_wait(
    A_EVENT *pEvent, A_EVENT_FLAGS flagsToWait, boolean waitAll, uint32_t timeout, A_EVENT_FLAGS *setFlags);
uint32_t a_time_get_msec(void);
#define OSA_Critical(x)      CPSR_ALLOC()
#define OSA_EnterCritical(x) { /*PRINTF("DISABLE IRQ\r\n");*/ RHINO_CRITICAL_ENTER(); }
#define OSA_ExitCritical(x)  { RHINO_CRITICAL_EXIT(); /*PRINTF("ENABLE IRQ\r\n");*/ }

#endif
