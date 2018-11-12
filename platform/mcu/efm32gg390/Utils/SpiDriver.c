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

#include "BCDS_SpiDriver.h"

/* Include RTOS support */
#include "BCDS_RTOS.h"

#include "BCDS_MCU_SPI.h"

static void SpiDriverEventCallback(SPI_T spi, struct MCU_SPI_Event_S event)
{
    SpiDriver_BusHandle_T parentHandle = NULL;

    if ((NULL == spi) ||
            (NULL == (parentHandle = MCU_SPI_GetParentHandle(spi)) /* parentHandle is extracted here */) ||
            (NULL == parentHandle->XferSyncSignal))
    {
        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
        return;
    }

    if (event.TxComplete || event.RxComplete)
    {
        /* No error noted and RX or TX have been completed */
        parentHandle->XferStatus = RETCODE_OK;
    }
    /* Test the Error flags */
    else if (event.TxError || event.RxError || event.DataLoss)
    {
        parentHandle->XferStatus = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    else
    {
        parentHandle->XferStatus = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNEXPECTED_BEHAVIOR);
    }

    /* The semaphore is given regardless of the error to ensure the continuity of the application */
    if (osOK != osSemaphoreRelease(parentHandle->XferSyncSignal))
    {
        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR));
    }
}

static Retcode_T SpiDriverSyncXferCallback(SpiDriver_BusHandle_T controllerHandle, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    if (osOK == osSemaphoreAcquire(controllerHandle->XferSyncSignal, timeout))
    {
        retcode = controllerHandle->XferStatus;
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMEOUT);
    }
    return retcode;
}

static inline Retcode_T SpiDriverValidateHandles(SpiDriver_DeviceHandle_T deviceHandle)
{
    if ((NULL == deviceHandle) ||
            (NULL == deviceHandle->Controller) ||
            (NULL == deviceHandle->Controller->XferSyncSignal) ||
            (NULL == deviceHandle->Controller->BusLock) ||
            (NULL == deviceHandle->Select) ||
            (NULL == deviceHandle->Deselect))
    {
        /* Device Handle is Null */
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else
    {
        return RETCODE_OK;
    }
}

Retcode_T SpiDriver_Initialize(SpiDriver_BusHandle_T busHandle)
{
    Retcode_T retcode = RETCODE_OK;

    if (NULL == busHandle)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    if (RETCODE_OK == retcode)
    {
        busHandle->BusLock = osMutexNew(NULL);
        if (NULL == busHandle->BusLock)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
        }
    }
    if (RETCODE_OK == retcode)
    {
        busHandle->XferSyncSignal = osSemaphoreNew(1UL, 0UL, NULL);
        if (NULL == busHandle->XferSyncSignal)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
        }
    }
    if (RETCODE_OK == retcode)
    {
        retcode = MCU_SPI_Initialize(busHandle->Spi, SpiDriverEventCallback);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = MCU_SPI_SetParentHandle(busHandle->Spi, busHandle);
    }
    return retcode;
}

Retcode_T SpiDriver_Deinitialize(SpiDriver_BusHandle_T busHandle)
{
    Retcode_T retcode = RETCODE_OK;

    if (NULL == busHandle)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = MCU_SPI_Deinitialize(busHandle->Spi);
    }
    if ((RETCODE_OK == retcode) && (NULL != busHandle->XferSyncSignal))
    {
        if (osOK == osSemaphoreDelete(busHandle->XferSyncSignal))
        {
            busHandle->XferSyncSignal = NULL;
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
        }
    }
    if ((RETCODE_OK == retcode) && (NULL != busHandle->BusLock))
    {
        if (osOK == osMutexDelete(busHandle->BusLock))
        {
            busHandle->BusLock = NULL;
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
        }
    }
    return retcode;
}

Retcode_T SpiDriver_Select(SpiDriver_DeviceHandle_T deviceHandle, void * selectParam, uint32_t timeout)
{
    Retcode_T retcode = SpiDriverValidateHandles(deviceHandle);

    if (RETCODE_OK == retcode)
    {
        if (osOK != osMutexAcquire(deviceHandle->Controller->BusLock, timeout))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMEOUT);
        }
    }
    if (RETCODE_OK == retcode)
    {
        retcode = deviceHandle->Select(selectParam);
    }
    return retcode;
}

Retcode_T SpiDriver_Deselect(SpiDriver_DeviceHandle_T deviceHandle, void * deselectParam)
{
    Retcode_T retcode = SpiDriverValidateHandles(deviceHandle);

    if (RETCODE_OK == retcode)
    {
        if (osOK != osMutexRelease(deviceHandle->Controller->BusLock))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
        }
    }
    if (RETCODE_OK == retcode)
    {
        retcode = deviceHandle->Deselect(deselectParam);
    }

    return retcode;
}

Retcode_T SpiDriver_Send(SpiDriver_DeviceHandle_T deviceHandle, void * pDataOut, size_t nDataOut, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;

    if ((NULL == pDataOut) || (0UL == nDataOut))
    {
        retcode = (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    }
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriverValidateHandles(deviceHandle);
    }
    if (RETCODE_OK == retcode)
    {
        if (osOK == osMutexAcquire(deviceHandle->Controller->BusLock, 0))
        {
            if (osOK != osMutexRelease(deviceHandle->Controller->BusLock))
            {
                Retcode_RaiseError(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR));
            }
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    if (RETCODE_OK == retcode)
    {
        /* Start the async transfer */
        retcode = MCU_SPI_Send((SPI_T) (deviceHandle->Controller->Spi), (uint8_t *) pDataOut, (uint32_t) nDataOut);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriverSyncXferCallback(deviceHandle->Controller, timeout);
    }
    return retcode;
}

Retcode_T SpiDriver_Receive(SpiDriver_DeviceHandle_T deviceHandle, void * pDataIn, size_t nDataIn, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;

    if ((NULL == pDataIn) || (0UL == nDataIn))
    {
        retcode = (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    }
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriverValidateHandles(deviceHandle);
    }
    if (RETCODE_OK == retcode)
    {
        if (osOK == osMutexAcquire(deviceHandle->Controller->BusLock, 0))
        {
            if (osOK != osMutexRelease(deviceHandle->Controller->BusLock))
            {
                Retcode_RaiseError(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR));
            }
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    if (RETCODE_OK == retcode)
    {
        /* Start the async transfer */
        retcode = MCU_SPI_Receive((SPI_T) (deviceHandle->Controller->Spi), (uint8_t*) pDataIn, (uint32_t) nDataIn);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriverSyncXferCallback(deviceHandle->Controller, timeout);
    }
    return retcode;
}

Retcode_T SpiDriver_Transfer(SpiDriver_DeviceHandle_T deviceHandle, void * pDataIn, void * pDataOut, size_t nDataInOut, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;

    if ((NULL == pDataIn) || (NULL == pDataOut) || (0UL == nDataInOut))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriverValidateHandles(deviceHandle);
    }
    if (RETCODE_OK == retcode)
    {
        if (osOK == osMutexAcquire(deviceHandle->Controller->BusLock, 0))
        {
            if (osOK != osMutexRelease(deviceHandle->Controller->BusLock))
            {
                Retcode_RaiseError(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR));
            }
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    if (RETCODE_OK == retcode)
    {
        /* Start the async transfer */
        retcode = MCU_SPI_Transfer((SPI_T) &(deviceHandle->Controller->Spi), (uint8_t*) pDataOut, (uint8_t*) pDataIn, (uint32_t) nDataInOut);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriverSyncXferCallback(deviceHandle->Controller, timeout);
    }
    return retcode;
}
