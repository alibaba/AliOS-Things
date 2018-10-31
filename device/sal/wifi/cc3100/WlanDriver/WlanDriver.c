/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/* module includes ********************************************************** */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_Wlan.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_WLAN_MODULE_ID_WLAN_DRIVER

/* own header files */
#include "BCDS_WlanDriver.h"
#include "WlanDriver.h"

/* additional interface header files */
#include "simplelink.h"
#include "BCDS_Assert.h"
//#include "FreeRTOS.h"
//#include "semphr.h"
//#include "timers.h"
#include "BCDS_MCU_SPI.h"
#include "BCDS_BSP_WiFi_CC3100MOD.h"
#include <aos/kernel.h>
#include <k_api.h>
#include <k_timer.h>
#include <k_queue.h>
#include <k_mutex.h>
#include <k_sem.h>
#include <k_time.h>
#include <port.h>
#include "stdio.h"

#define PRINT_LOG_FEATURE   0

/* constant definitions ***************************************************** */
#define BIT_0 ( 1 << 0 )

/* local variables ***********************************************************/
//lint -e956 Suppressing Non const, non volatile static or external variable lint warning
/** Queue handle for the Queue created for Spawn task */
static kbuf_queue_t SpawnQueueHandle;
/** Task handle for Spawn task */
static ktask_t SpawnTaskHandle;
/** Event Handler declaration required by wifi stack */
static WLANDRIVER_EVENT_HANDLER WlanIrqEventHandler = NULL;
/** SPI handle for hardware SPI instance created for wifi */
static SPI_T spiHandle;
/* semaphore unblocking completion of frame transmission*/
static ksem_t * spiCompleteSync;

/* global variables ********************************************************* */

//lint +e956 enabling Non const, non volatile static or external variable lint warning
/* inline functions ********************************************************* */

/* local functions ********************************************************** */

static inline uint64_t OsTime2Tick(WlanDriver_OsTime TimeOut)
{
    if (0 == TimeOut)
    {
        return 0;
    }
    else if (WLANDRIVER_OS_WAIT_FOREVER == TimeOut)
    {
        return RHINO_WAIT_FOREVER;
    }
    else
    {
        return krhino_ms_to_ticks(TimeOut);
    }
}

/**
 * @brief        Function to register SPI callback for interrupt
 *
 * @param [in]   SPI_T: SPI Handle
 *
 * @param [in]   MCU_SPI_Event_S: structure holding the SPI events
 */
static void spiEventsCallbackFunc(SPI_T spi, struct MCU_SPI_Event_S event)
{
#if PRINT_LOG_FEATURE
    printf("spiEventsCallbackFunc\r\n");
#endif
//    if (spiCompleteSync != NULL)
//    {
    if ((event.TxComplete) || (event.RxComplete))
    {
        if (krhino_sem_give(spiCompleteSync) != RHINO_SUCCESS)
        {
            printf("krhino_event_set failure\r\n");
        }
        else
        {
            printf("krhino_event_set success\r\n");
        }
        return;
    }

//    }
//    else
//    {
//        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_NULL_POINTER));
//    }
    if ((SPI_T) 0 == spi)
    {        
        printf("(SPI_T) 0 == spi\r\n");
        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_WLANDRIVER_NULL_HANDLE));
    }

    if (event.RxError)
    {
        printf("event.RxError\r\n");
        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_WLANDRIVER_SPI_RX_ERROR));
    }

    if (event.TxError)
    {
        printf("event.TxError\r\n");
        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_WLANDRIVER_SPI_TX_ERROR));
    }

    if (event.DataLoss)
    {
        printf("event.DataLoss\r\n");
        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_WLANDRIVER_SPI_DATALOSS));
    }
}

static cpu_stack_t KernelInitStackBuffer[WID_SPAWN_TASK_STACK_SIZE];

static uint8_t spawnDataBuffer[WID_SPAWN_QUEUE_SIZE * sizeof(WlanDriver_SlSpawnMsg_T)] = { 0 };

/** @brief This function is to create spawn Task and queue needed by simpleLink driver
 *  @param[in] spawnTaskPriority
 *             priority of the spawn task
 *  @return
 *         OS_ERR_NO_ERROR - on success
 *         OS_ERR_NOT_ENOUGH_MEMORY - on error in creating spawn task
 */
static int32_t CreateSpawnQAndTask(uint32_t spawnTaskPriority)
{
#if PRINT_LOG_FEATURE
    printf("CreateSpawnQAndTask\r\n");
#endif
    int8_t retVal = WLANDRIVER_NOT_ENOUGH_MEMORY;

    if (RHINO_SUCCESS == krhino_buf_queue_create(&SpawnQueueHandle, "SpawnQueue", &spawnDataBuffer[0], WID_SPAWN_QUEUE_SIZE * sizeof(WlanDriver_SlSpawnMsg_T), sizeof(WlanDriver_SlSpawnMsg_T)))
    {
        if (RHINO_SUCCESS != krhino_task_create(&SpawnTaskHandle, "SLSPAWN", 0, spawnTaskPriority, 0, KernelInitStackBuffer, WID_SPAWN_TASK_STACK_SIZE, (task_entry_t) WlanDriver_SimpleLinkSpawnTask, 1))
        {
            /* The task was not created as there was insufficient heap memory remaining.*/
            retVal = WLANDRIVER_NOT_ENOUGH_MEMORY;
        }
        else
        {
//            cpu_task_switch();
//            aos_msleep(OsTime2Tick(100)); /* For context switch. Without this, the context switch doesn't happen. */
            retVal = WLANDRIVER_SUCCESS;
        }
    }
    else
    {
        retVal = WLANDRIVER_NOT_ENOUGH_MEMORY;
    }
    return (retVal);
}

/**
 *  @brief
 *      This is the callback function that got triggered by WiFi device interrupt pin.
 *  @usage
 *      This function will be used by BSP_WiFi_CC3100MOD_Enable()
 */
static void WifiInterruptCallback(void)
{
#if PRINT_LOG_FEATURE
    printf("WifiInterruptCallback\r\n");
#endif
    if (WlanIrqEventHandler != NULL)
    {
        WlanIrqEventHandler(0);
    }
}

/* global functions ********************************************************* */
/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
Retcode_T WlanDriver_Init(void)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_Init\r\n");
#endif
    Retcode_T ret = RETCODE_OK;
    int32_t returnValue = WLANDRIVER_FAILURE;
    uint32_t spawnTaskPriority = WID_SPAWN_TASK_PRIORITY;

    ret = BSP_WiFi_CC3100MOD_Connect();
    printf("WlanDriver_Init ret=%d\r\n",ret);

    if (RETCODE_OK == ret)
    {
//        if (NULL == spiCompleteSync)
        {
            /* @todo - check the return code */
            if (RHINO_SUCCESS != krhino_sem_dyn_create(&spiCompleteSync, "spiCompleteSync", 0))
            {
                ret = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
            }
//            else
//            {
                /* dummy take */
//                (void)krhino_sem_take(&spiCompleteSync, 0);
//            }
        }
//        if (NULL != spiCompleteSync)
        {
            spiHandle = BSP_WiFi_CC3100MOD_GetSPIHandle();

            returnValue = CreateSpawnQAndTask(spawnTaskPriority);

            if (WLANDRIVER_SUCCESS != returnValue)
            {
                ret = (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_RTOS_QUEUE_ERROR));
            }
        }
    }

    return ret;
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
int WlanDriver_RegInterruptHdlr(WLANDRIVER_EVENT_HANDLER interruptHandle, void* pValue)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_RegInterruptHdlr\r\n");
#endif
    BCDS_UNUSED(pValue);
    if (interruptHandle == NULL)
    {
        return ((int) WLANDRIVER_FAILURE);
    }
    WlanIrqEventHandler = interruptHandle;
    return ((int) WLANDRIVER_SUCCESS);
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
void WlanDriver_SlHttpServerCb(SlHttpServerEvent_t *pHttpEvent,
        SlHttpServerResponse_t *pHttpResponse)
{
    BCDS_UNUSED(pHttpEvent);
    BCDS_UNUSED(pHttpResponse);
}

/* For details refer API documentation is in the configuration header file User.h  */
void __attribute__((weak)) Wlan_SlSockEvtHdlr(SlSockEvent_t * pSlSockEvent)
{
    BCDS_UNUSED(pSlSockEvent);
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
Retcode_T WlanDriver_DeviceDisable(void)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_DeviceDisable\r\n");
#endif
    return BSP_WiFi_CC3100MOD_Disable();
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
Retcode_T WlanDriver_DeviceEnable(void)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_DeviceEnable\r\n");
#endif

    return BSP_WiFi_CC3100MOD_Enable(WifiInterruptCallback);
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
int WlanDriver_SpiClose(Fd_t fd)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_SpiClose\r\n");
#endif
    BCDS_UNUSED(fd);

    if ((SPI_T) 0 == spiHandle)
    {
        return (WLANDRIVER_SPI_ERROR);
    }

    return (int) MCU_SPI_Deinitialize(spiHandle);
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
Fd_t WlanDriver_SpiOpen(char *ifName, unsigned long flags)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_SpiOpen\r\n");
#endif

    Retcode_T retcode;
    BCDS_UNUSED(ifName);
    BCDS_UNUSED(flags);

    if ((SPI_T) 0 == spiHandle)
    {
        return (WLANDRIVER_SPI_ERROR);
    }

    retcode = MCU_SPI_Initialize(spiHandle, spiEventsCallbackFunc);
    if (RETCODE_OK != retcode)
    {
        return WLANDRIVER_SPI_ERROR;
    }
    else
    {
        return WLANDRIVER_SUCCESS;
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
int WlanDriver_SpiWrite(Fd_t fd, unsigned char *pBuff, int len)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_SpiWrite\r\n");
#endif
    BCDS_UNUSED(fd);

    //assert(pBuff != NULL);

#if PRINT_LOG_FEATURE
    printf("WlanDriver_SpiWrite Request (byte count - %d) - ", (unsigned int) len);

    for (int i = 0; i < len; i++)
    {
        printf("0x%02X ", pBuff[i]);
    }
    printf("\r\n");
#endif

    Retcode_T ret = RETCODE_OK;
    int wlanRet = WLANDRIVER_SPI_RW_ERROR;
//    if ((len > 0) && (spiCompleteSync != NULL))
    if (len > 0)
    {
        (void) BSP_WiFi_CC3100MOD_SetCSLow();

        /* Dummy take */
        (void)krhino_sem_take(spiCompleteSync, 0);

        ret = MCU_SPI_Send(spiHandle, (uint8_t*) pBuff, (uint32_t) len);
 
        if (RETCODE_OK == ret)
        {
            if (RHINO_SUCCESS != krhino_sem_take(spiCompleteSync, OsTime2Tick(WLANDRIVER_COM_TIMEOUT)))
            {
                
                ret = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_TIMEOUT);
                
             }
        }

        (void) BSP_WiFi_CC3100MOD_SetCSHigh();
        if (RETCODE_OK == ret)
        {
            wlanRet = len;
        }
        else
        {
            wlanRet = (WLANDRIVER_SPI_RW_ERROR);
        }
    }

#if PRINT_LOG_FEATURE
    printf("WlanDriver_SpiWrite Response - %d\r\n", (unsigned int) wlanRet);
#endif

    return (wlanRet);
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
int WlanDriver_SpiRead(Fd_t fd, unsigned char *pBuff, int len)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_SpiRead\r\n");
#endif
    BCDS_UNUSED(fd);

    //assert(pBuff != NULL);

#if PRINT_LOG_FEATURE
    printf("WlanDriver_SpiRead Request - %d\r\n", (unsigned int) len);

    //    if((NULL == pBuff) || (0 == len))
    //        {
    //        return 0;
    //        }
#endif

    Retcode_T ret = RETCODE_OK;
    int wlanRet = WLANDRIVER_SPI_RW_ERROR;
//    if ((len > 0) && (spiCompleteSync != NULL))
    if (len > 0)
    {
        (void) BSP_WiFi_CC3100MOD_SetCSLow();
        /* Dummy take */
        (void)krhino_sem_take(spiCompleteSync, 0);
        
         ret = MCU_SPI_Receive(spiHandle, (uint8_t*) pBuff, (uint32_t) len);
         if (RETCODE_OK == ret)
        {
            if (RHINO_SUCCESS != krhino_sem_take(spiCompleteSync, OsTime2Tick(WLANDRIVER_COM_TIMEOUT)))
            {
                ret = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_TIMEOUT);
            }
        }
        (void) BSP_WiFi_CC3100MOD_SetCSHigh();
        
         if (RETCODE_OK == ret)
        {
            wlanRet = len;
        }
        else
        {
            wlanRet = (WLANDRIVER_SPI_RW_ERROR);
        }
    }

#if PRINT_LOG_FEATURE
    printf("WlanDriver_SpiRead Response (byte count %d) - ", (unsigned int) wlanRet);
    for (int i = 0; i < wlanRet; i++)
    {
        printf("0x%02X ", pBuff[i]);
    }
    printf("\r\n");
#endif
     return (wlanRet);
}

int32_t WlanDriver_OsSyncObjectCreate(WlanDriver_OsSyncObject* pSyncObj)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsSyncObjectCreate\r\n");
#endif
    if ((NULL != pSyncObj) && (RHINO_SUCCESS == krhino_sem_create(pSyncObj, "DUMMY", 1)))
    {
        /* dummy take */
        krhino_sem_take(pSyncObj, 0);
        return (WLANDRIVER_SUCCESS);
    }
    else
    {
        return (WLANDRIVER_FAILURE);
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsSyncObjectDelete(WlanDriver_OsSyncObject* pSyncObj)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsSyncObjectDelete\r\n");
#endif
    if ((NULL != pSyncObj) && (RHINO_SUCCESS == krhino_sem_del(pSyncObj)))
    {
        return (WLANDRIVER_SUCCESS);
    }
    else
    {
        return (WLANDRIVER_FAILURE);
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsSyncObjectSignal(WlanDriver_OsSyncObject* pSyncObj)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsSyncObjectSignal\r\n");
#endif
    if ((NULL != pSyncObj) && (RHINO_SUCCESS == krhino_sem_give(pSyncObj)))
    {
        return (WLANDRIVER_SUCCESS);
    }
    else
    {
        return (WLANDRIVER_FAILURE);
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsSyncSignalFromISR(WlanDriver_OsSyncObject* pSyncObj)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsSyncSignalFromISR\r\n");
#endif
    if ((NULL != pSyncObj) && (RHINO_SUCCESS == krhino_sem_give(pSyncObj)))
    {
        return (WLANDRIVER_SUCCESS);
    }
    else
    {
        return (WLANDRIVER_FAILURE);
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsSyncObjectWait(WlanDriver_OsSyncObject* pSyncObj, WlanDriver_OsTime TimeOut)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsSyncObjectWait\r\n");
#endif
    if (NULL == pSyncObj)
    {
        return WLANDRIVER_FAILURE;
    }
    if (krhino_sem_take(pSyncObj, OsTime2Tick(TimeOut)) == RHINO_SUCCESS)
    {
        return (WLANDRIVER_SUCCESS);
    }
    else
    {
        return (WLANDRIVER_FAILURE);
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsSyncObjectClear(WlanDriver_OsSyncObject* pSyncObj)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsSyncObjectClear\r\n");
#endif
    if (NULL == pSyncObj)
    {
        return WLANDRIVER_FAILURE;
    }
    if (WlanDriver_OsSyncObjectWait(pSyncObj, WLANDRIVER_OS_NO_WAIT) == WLANDRIVER_SUCCESS)
    {
        return (WLANDRIVER_SUCCESS);
    }
    else
    {
        return (WLANDRIVER_FAILURE);
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsLockObjectCreate(WlanDriver_OsLockObject* pLockObj)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsLockObjectCreate\r\n");
#endif
    if ((pLockObj != NULL) && (RHINO_SUCCESS == krhino_mutex_create(pLockObj, "DUMMY")))
    {
        
         return (WLANDRIVER_SUCCESS);
    }
    else
    {
        
         return (WLANDRIVER_FAILURE);
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsLockObjectDelete(WlanDriver_OsLockObject* pLockObj)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsLockObjectDelete\r\n");
#endif
    if ((pLockObj != NULL) && (RHINO_SUCCESS == krhino_mutex_del(pLockObj)))
    {
        return (WLANDRIVER_SUCCESS);
    }
    else
    {
        return (WLANDRIVER_FAILURE);
    }

}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsLockObjectWait(WlanDriver_OsLockObject* pLockObj, WlanDriver_OsTime TimeOut)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsLockObjectWait\r\n");
#endif
    if (pLockObj == NULL)
    {
        return (WLANDRIVER_FAILURE);
    }

    if (krhino_mutex_lock(pLockObj, OsTime2Tick(TimeOut)) == RHINO_SUCCESS)
    {
        return (WLANDRIVER_SUCCESS);
    }
    else
    {
        return (WLANDRIVER_FAILURE);
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h */
int32_t WlanDriver_OsLockUnlock(WlanDriver_OsLockObject* pLockObj)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsLockUnlock\r\n");
#endif
    if (NULL == pLockObj)
    {
        
         return (WLANDRIVER_FAILURE);
    }
    if (krhino_mutex_unlock(pLockObj) == RHINO_SUCCESS)
    {
        
         return (WLANDRIVER_SUCCESS);
    }
    else
    {
        
         return (WLANDRIVER_FAILURE);
    }
    
 }

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
void WlanDriver_SimpleLinkSpawnTask(void *pvParameters)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_SimpleLinkSpawnTask\r\n");
#endif
    BCDS_UNUSED(pvParameters);
    WlanDriver_SlSpawnMsg_T message;
    for (;;)
    {
        size_t len = 0;
        /* @todo - Check if RHINO_WAIT_FOREVER is to be used. */
        if (RHINO_SUCCESS == krhino_buf_queue_recv(&SpawnQueueHandle, MAX_TIMER_TICKS, &message, &len))
        {
            if (len != 0)
            {
#if PRINT_LOG_FEATURE
                printf("WlanDriver_SimpleLinkSpawnTask %x : %x\r\n", (unsigned int) *message.pEntry, (unsigned int) message.pValue);
#endif
                if (0 != message.pEntry(message.pValue))
                {
                     Retcode_RaiseError(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE));
                }
#if PRINT_LOG_FEATURE
                printf("WlanDriver_SimpleLinkSpawnTask exit\r\n");
#endif
            }
            else
            {
#if PRINT_LOG_FEATURE
                printf("WlanDriver_SimpleLinkSpawnTask failed 1\r\n");
#endif
                Retcode_RaiseError(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_RTOS_QUEUE_ERROR));
            }
        }
        else
        {
#if PRINT_LOG_FEATURE
            printf("WlanDriver_SimpleLinkSpawnTask failed 2\r\n");
#endif
            Retcode_RaiseError(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_RTOS_QUEUE_ERROR));
        }
    }
}

/* For details refer API documentation is in the interface header file BCDS_WlanDriver.h  */
int32_t WlanDriver_OsiSpawn(WLANDRIVER_OSI_SPAWN_ENTRY pEntry, void* pValue, unsigned long flags)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_OsiSpawn\r\n");
#endif
    BCDS_UNUSED(flags);
    WlanDriver_SlSpawnMsg_T message;
    message.pEntry = pEntry;
    message.pValue = pValue;

    if (SL_SPAWN_FLAG_FROM_SL_IRQ_HANDLER == flags)
    {
        if (RHINO_SUCCESS == krhino_buf_queue_send(&SpawnQueueHandle, &message, sizeof(message)))
        {
            cpu_intrpt_switch();
#if PRINT_LOG_FEATURE
            printf("WlanDriver_OsiSpawn %x : %x\r\n", (unsigned int) *message.pEntry, (unsigned int) message.pValue);
#endif
            return (WLANDRIVER_SUCCESS);
        }
        else
        {
#if PRINT_LOG_FEATURE
            printf("WlanDriver_OsiSpawn failed\r\n");
#endif
            return (WLANDRIVER_FAILURE);
        }
    }
    else
    {
        if (RHINO_SUCCESS == krhino_buf_queue_send(&SpawnQueueHandle, &message, sizeof(message)))
        {
            cpu_task_switch();
#if PRINT_LOG_FEATURE
            printf("WlanDriver_OsiSpawn %x : %x\r\n", (unsigned int) *message.pEntry, (unsigned int) message.pValue);
#endif
            return (WLANDRIVER_SUCCESS);
        }
        else
        {
#if PRINT_LOG_FEATURE
            printf("WlanDriver_OsiSpawn failed\r\n");
#endif
            return (WLANDRIVER_FAILURE);
        }
    }
}

/** Refer interface header for description */
Retcode_T WlanDriver_SetDeviceTime(struct tm *tm)
{
    Retcode_T retcode = RETCODE_OK;
    SlDateTime_t dateTime = { 0 };

    if (NULL == tm)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else
    {
        dateTime.sl_tm_day = tm->tm_mday;
        dateTime.sl_tm_mon = tm->tm_mon + 1; /* WLAN chip expects month to start from 1 (instead of the standard 0) */
        dateTime.sl_tm_year = tm->tm_year + 1900; /* WLAN chip expects year to start from 0 (instead of the standard 1900) */
        dateTime.sl_tm_hour = tm->tm_hour;
        dateTime.sl_tm_min = tm->tm_min;
        dateTime.sl_tm_sec = tm->tm_sec;
        if (0 != sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION, SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME, sizeof(SlDateTime_t), (unsigned char *) (&dateTime)))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
        }
    }

    return retcode;
}

Retcode_T WlanDriver_DeInit(void)
{
#if PRINT_LOG_FEATURE
    printf("WlanDriver_DeInit\r\n");
#endif
    Retcode_T ret = RETCODE_OK;

    ret = BSP_WiFi_CC3100MOD_Disconnect();

    if (RETCODE_OK == ret)
    {
//        if (NULL != spiCompleteSync)
        {
            (void) krhino_sem_del(spiCompleteSync);
//            spiCompleteSync = NULL;
        }
//        if (NULL != SpawnTaskHandle)
//        {
        /* Delete spawn task and Queue  */
        /* @todo - check return value */
        (void) krhino_task_del(&SpawnTaskHandle);
//            SpawnTaskHandle = NULL;
//        }
//        if (NULL != SpawnQueueHandle)
        {
            /* Delete spawn Queue  */
            krhino_buf_queue_del(&SpawnQueueHandle);
//            SpawnQueueHandle = NULL;
        }
    }

    return ret;
}
/** ************************************************************************* */
