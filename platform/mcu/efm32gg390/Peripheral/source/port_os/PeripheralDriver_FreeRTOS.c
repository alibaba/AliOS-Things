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

/* Include all headers which are needed by this file in the following order:
 * Own public header
 * Own protected header
 * Own private header
 * System header (if necessary)
 * Other headers
 */
#include "BCDS_PeripheralDriver.h"   /*public definitions */
#include "../PeripheralDriver.h"     /*private interface */

#include "BCDS_Basics.h"
#include "BCDS_Retcode.h"

#include "FreeRTOS.h"
#include "semphr.h"

/* Put the type and macro definitions here */

/* Put constant and variable definitions here */

/* Put private function declarations here */

#if configSUPPORT_DYNAMIC_ALLOCATION
static Retcode_T PeripheralDriver_initializeDynamic(PeripheralDriver_Handle_T peripheral);
#endif

/* Put function implementations here */

#if configSUPPORT_DYNAMIC_ALLOCATION
static Retcode_T PeripheralDriver_initializeDynamic(PeripheralDriver_Handle_T peripheral)
{
    Retcode_T retcode = RETCODE_OK;
    assert(peripheral!=NULL);
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    SemaphoreHandle_t lock = NULL;
    SemaphoreHandle_t signal = NULL;

    signal = xQueueCreate(1, sizeof(uint32_t));
    if (NULL == signal)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
    }
    if (RETCODE_OK == retcode)
    {
        lock = xSemaphoreCreateMutex();
        if (lock == NULL)
        {
            /* clean up previously allocated semaphore */
            vSemaphoreDelete(signal);
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
        }
    }
    if (RETCODE_OK == retcode)
    {
        /* first assign the signal, which won't cause any critical race */
        peripheralPtr->private.Signal = signal;
        peripheralPtr->private.Lock = lock;
    }
    else
    {
        /* for security set the signals to null */
        peripheralPtr->private.Signal = NULL;
        peripheralPtr->private.Lock = NULL;

    }
    return retcode;
}

#endif

Retcode_T PeripheralDriver_initialize(PeripheralDriver_Handle_T peripheral)
{
    Retcode_T retcode = RETCODE_OK;

    assert(peripheral!=NULL);
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;

    if (peripheralPtr->private.Lock || peripheralPtr->private.Signal)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_DOUBLE_INITIALIZATION);
    }
    if (RETCODE_OK == retcode)
    {

#if configSUPPORT_STATIC_ALLOCATION
        retcode = PeripheralDriver_initializeStatic(peripheral);
#elif configSUPPORT_DYNAMIC_ALLOCATION
        retcode = PeripheralDriver_initializeDynamic(peripheral);
#else
#error("PeripheralDriver_initialize: FreeRTOS config error: define allocation method")
#endif
    }
    return retcode;

}

Retcode_T PeripheralDriver_open(PeripheralDriver_Handle_T peripheral, uint32_t timeout)
{
    assert(peripheral!=NULL);
    Retcode_T retcode = RETCODE_OK;
    BaseType_t libreturn;
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;

    if (NULL == peripheralPtr->private.Lock)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
    }
    if (RETCODE_OK == retcode)
    {
        libreturn = xSemaphoreTake(peripheralPtr->private.Lock, timeout);
        if (libreturn == pdFAIL)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMEOUT);
        }
    }
    return retcode;
}

Retcode_T PeripheralDriver_close(PeripheralDriver_Handle_T peripheral)
{
    assert(peripheral!=NULL);
    Retcode_T retcode = RETCODE_OK;
    BaseType_t libreturn;
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    if (NULL == peripheralPtr->private.Lock)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
    }
    if (RETCODE_OK == retcode)
    {
        libreturn = xSemaphoreGive(peripheralPtr->private.Lock);
        if (libreturn == pdFAIL)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
        }

    }
    return retcode;

}

Retcode_T PeripheralDriver_deinitialize(PeripheralDriver_Handle_T peripheral, uint32_t timeout)
{
    assert(peripheral!=NULL);
    Retcode_T retcode = RETCODE_OK;
    SemaphoreHandle_t lock;
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;

    /* we store the lock here to be used later. */
    lock = peripheralPtr->private.Lock;

    /* now remove the lock from the the driver context, a previously opened device may still claim it concurrently */
    peripheralPtr->private.Lock = NULL;
    /* now we claim the mutex */
    if (pdTRUE == xSemaphoreTake(lock, timeout))
    {
        /* now it is safe to delete the mutex as no one can claim it through the driver */
        vSemaphoreDelete(lock);
        vSemaphoreDelete(peripheralPtr->private.Signal);
    }
    else
    {
        /* if we cannot claim the peripheral driver, restore the driver context */
        peripheralPtr->private.Lock = lock;
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_DEINITIALIZE_FAILED);
    }

    return retcode;

}

Retcode_T PeripheralDriver_waitSignal(PeripheralDriver_Handle_T peripheral, uint32_t *event, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    BaseType_t libresult = 0;

    /* read the queue handle from the structure */
    QueueHandle_t queue = peripheralPtr->private.Signal;
    if (NULL != queue)
    {
        libresult = xQueueReceive(queue, &event, timeout);
        if (pdFALSE == libresult)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMEOUT);
        }
    }
    return retcode;
}

Retcode_T PeripheralDriver_signal(PeripheralDriver_Handle_T peripheral, uint32_t event)
{
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    BaseType_t libresult = 0, xHigherPriorityTaskWoken = 0;

    /* read the queue handle from the structure */
    QueueHandle_t queue = peripheralPtr->private.Signal;
    if (queue)
    {
        libresult = xQueueSendFromISR(queue, &event, &xHigherPriorityTaskWoken);
        if (libresult)
        {
            // Actual macro used here is port specific.
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    return RETCODE_OK;
}

Retcode_T PeripheralDriver_resetSignal(PeripheralDriver_Handle_T peripheral)
{
    Retcode_T retcode = RETCODE_OK;
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    BaseType_t libresult = 0;

    /* read the queue handle from the structure */
    QueueHandle_t queue = peripheralPtr->private.Signal;
    if (queue)
    {
        libresult = xQueueReset(queue);
        if (pdFALSE == libresult)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
        }
    }
    return retcode;

}

