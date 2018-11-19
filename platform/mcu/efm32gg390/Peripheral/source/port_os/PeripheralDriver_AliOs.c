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

#include "PeripheralDriver_AliOs.h"
#include "BCDS_Peripheral.h"
#include <k_api.h>

/* Put the type and macro definitions here */

/* Put constant and variable definitions here */

/* Put private function declarations here */

static Retcode_T PeripheralDriver_initializeDynamic(PeripheralDriver_Handle_T peripheral);

/* Put function implementations here */

static Retcode_T PeripheralDriver_initializeDynamic(PeripheralDriver_Handle_T peripheral)
{
    Retcode_T retcode = RETCODE_OK;
    assert(peripheral!=NULL);
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    kmutex_t * lock = NULL;
    kbuf_queue_t * signal = NULL;

    if (RHINO_SUCCESS != krhino_buf_queue_dyn_create(&signal, "signal", 4 * sizeof(uint32_t), sizeof(uint32_t)))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
    }
    if (RETCODE_OK == retcode)
    {
        if (RHINO_SUCCESS != krhino_mutex_dyn_create(&lock, "lock"))
        {
            /* clean up previously allocated semaphore */
            (void) krhino_buf_queue_dyn_del(signal);
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

        retcode = PeripheralDriver_initializeDynamic(peripheral);
    }
    return retcode;

}

Retcode_T PeripheralDriver_open(PeripheralDriver_Handle_T peripheral, uint32_t timeout)
{
    assert(peripheral!=NULL);
    Retcode_T retcode = RETCODE_OK;
    kstat_t libreturn;
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;

    if (NULL == peripheralPtr->private.Lock)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
    }
    if (RETCODE_OK == retcode)
    {
        libreturn = krhino_mutex_lock(peripheralPtr->private.Lock, krhino_ms_to_ticks(timeout));
        if (RHINO_SUCCESS != libreturn)
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
    kstat_t libreturn;
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    if (NULL == peripheralPtr->private.Lock)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
    }
    if (RETCODE_OK == retcode)
    {
        libreturn = krhino_mutex_unlock(peripheralPtr->private.Lock);
        if (RHINO_SUCCESS != libreturn)
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
    kmutex_t *lock;
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;

    /* we store the lock here to be used later. */
    lock = peripheralPtr->private.Lock;

    /* now remove the lock from the the driver context, a previously opened device may still claim it concurrently */
    peripheralPtr->private.Lock = NULL;
    /* now we claim the mutex */
    if (RHINO_SUCCESS == krhino_mutex_lock(lock, krhino_ms_to_ticks(timeout)))
    {
        /* now it is safe to delete the mutex as no one can claim it through the driver */
        (void) krhino_mutex_del(lock);
        (void) krhino_buf_queue_dyn_del(peripheralPtr->private.Signal);
        peripheralPtr->private.Signal = NULL;

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
    kstat_t libresult = RHINO_SUCCESS;

    /* read the queue handle from the structure */
    kbuf_queue_t *queue = peripheralPtr->private.Signal;
    if (NULL != queue)
    {
        size_t len = 0;
        libresult = krhino_buf_queue_recv(queue, krhino_ms_to_ticks(timeout), event, &len);
        if ((RHINO_SUCCESS != libresult) && (len == 0))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMEOUT);
        }
    }
    return retcode;
}

Retcode_T PeripheralDriver_signal(PeripheralDriver_Handle_T peripheral, uint32_t event)
{
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    kstat_t libresult = RHINO_SUCCESS;

    /* read the queue handle from the structure */
    kbuf_queue_t *queue = peripheralPtr->private.Signal;
    if (queue)
    {
        libresult = krhino_buf_queue_send(queue, &event, sizeof(event));
        if (RHINO_SUCCESS == libresult)
        {
            // Actual macro used here is port specific.
            cpu_intrpt_switch();
        }
    }

    return RETCODE_OK;
}

Retcode_T PeripheralDriver_resetSignal(PeripheralDriver_Handle_T peripheral)
{
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    PeripheralDriverPtr_T peripheralPtr = (PeripheralDriverPtr_T) peripheral;
    kstat_t libresult = RHINO_SUCCESS;

    /* read the queue handle from the structure */
    kbuf_queue_t *queue = peripheralPtr->private.Signal;
    if (queue)
    {
        libresult = krhino_buf_queue_flush(queue);
        if (RHINO_SUCCESS != libresult)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
        }
        else
        {
            retcode = RETCODE_OK;
        }
    }
    return retcode;

}

