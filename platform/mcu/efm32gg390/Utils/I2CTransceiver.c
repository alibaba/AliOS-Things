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

#include "BCDS_Utils.h"

#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_UTILS_MODULE_ID_I2C_TRANSCEIVER

#if BCDS_FEATURE_I2CTRANSCEIVER
/* Put the type and macro definitions here */
#include "BCDS_I2CTransceiver.h"
#if BCDS_FEATURE_I2C
/* Put constant and variable definitions here */
#define CANCEL_I2C_TRANSMISSION  UINT32_C(0)
#define DATA_TRANSFER_TIMEOUT_MS  UINT32_C(1000)

/* Put private function declarations here */

/* Private function definitions */

/* Put function implementations here */

/* API documentation is in the interface header file */
void I2CTransceiver_LoopCallback(I2cTranceiverHandlePtr_T i2cTransceiver, struct MCU_I2C_Event_S event)
{
    Retcode_T retcode = RETCODE_OK;
//    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if (NULL == i2cTransceiver)
    { /* Handle is not initialized */
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    if (RETCODE_OK == retcode)
    {
        if (false == i2cTransceiver->InitializationStatus)
        { /* Transciever is not initialized */
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
        }
//        else if (NULL == i2cTransceiver->I2CBusSync)
//        { /* I2C Bus lock not present */
//            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
//        }
    }

    if (RETCODE_OK == retcode)
    {
        if (UINT8_C(1) == event.TxComplete)
        {
            i2cTransceiver->I2cTransferStatusFlag = INT8_C(0); // No Error in I2C Transfer Flagged
        }

        else if (UINT8_C(1) == event.TransferError)
        {
            i2cTransceiver->I2cTransferStatusFlag = INT8_C(-1); //  Error in I2C Transfer Flagged
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_I2CTRANSCEIVER_TRANSFER_ERROR);
        }
        /* Give Semaphore even in case of transfer errors or Transfer completed */
        if (RHINO_SUCCESS == krhino_sem_give(&i2cTransceiver->I2CBusSync))
        {
//            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }
        else
        {
            /* ignore... semaphore has already been given */
        }
    }
    if (RETCODE_OK != retcode)
    {
        Retcode_RaiseErrorFromIsr(retcode);
    }
}

/* API documentation is in the interface header file */
Retcode_T I2CTransceiver_Init(I2cTranceiverHandlePtr_T i2cTransceiver, I2C_T i2cHandle)
{
    Retcode_T retcode = RETCODE_OK;
    if ((NULL == i2cTransceiver) || (NULL == i2cHandle))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else
    {
        if (false == i2cTransceiver->InitializationStatus)
        {

            i2cTransceiver->I2CHandle = i2cHandle;
            krhino_sem_create(&i2cTransceiver->I2CBusSync, "DUMMY", 1);
            /* dummy take */
            krhino_sem_take(&i2cTransceiver->I2CBusSync, 0);
            krhino_mutex_create(&i2cTransceiver->I2CMutexLock, "DUMMY");/* = xSemaphoreCreateMutex();*/
//            if ((NULL != i2cTransceiver->I2CBusSync) && (NULL != i2cTransceiver->I2CMutexLock))
//            {
            i2cTransceiver->InitializationStatus = true;
//            }
//            else if ((NULL != i2cTransceiver->I2CBusSync) && (NULL == i2cTransceiver->I2CMutexLock))
//            {
//                vSemaphoreDelete(i2cTransceiver->I2CBusSync);
//                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
//            }
//            else if ((NULL == i2cTransceiver->I2CBusSync) && (NULL != i2cTransceiver->I2CMutexLock))
//            {
//                vSemaphoreDelete(i2cTransceiver->I2CMutexLock);
//                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
//            }
//            else
//            {
//                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
//            }
        }
        else
        {
            retcode = RETCODE_OK;

        }
    }

    return retcode;
}

/* API documentation is in the interface header file */
Retcode_T I2CTransceiver_Read(I2cTranceiverHandlePtr_T i2cTransceiver, uint8_t I2Caddr, uint8_t Reg, uint8_t *RegData, uint8_t NbrBytesToRead)
{
    Retcode_T retcode = RETCODE_OK;

    if (UINT8_C(0) == NbrBytesToRead)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    }

//    if ((NULL == i2cTransceiver) || (NULL == RegData) || (NULL == i2cTransceiver->I2CBusSync) || (NULL == i2cTransceiver->I2CMutexLock) || (NULL == i2cTransceiver->I2CHandle))
//    {
//        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
//    }

    if (false == i2cTransceiver->InitializationStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED));
    }
    if (RHINO_SUCCESS != krhino_mutex_lock(&i2cTransceiver->I2CMutexLock, /*(TickType_t) pdMS_TO_TICKS*/(DATA_TRANSFER_TIMEOUT_MS)))
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR));
    }
    
    retcode = MCU_I2C_ReadRegister(i2cTransceiver->I2CHandle, (uint16_t) I2Caddr, Reg, RegData, NbrBytesToRead);
    if (RETCODE_OK == retcode)
    {
        if (RHINO_SUCCESS != krhino_sem_take(&i2cTransceiver->I2CBusSync, /*(TickType_t) pdMS_TO_TICKS*/(DATA_TRANSFER_TIMEOUT_MS)))
        {
            /* Since the I2C transfer time out happened, Abort an ongoing I2C transmission.*/
            retcode = MCU_I2C_Send(i2cTransceiver->I2CHandle, (uint16_t) I2Caddr, RegData, CANCEL_I2C_TRANSMISSION);
            if (RETCODE_OK == retcode)
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMEOUT);
            }
        }
        else
        {
            if (INT8_C(-1) == i2cTransceiver->I2cTransferStatusFlag)
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_I2CTRANSCEIVER_TRANSFER_ERROR);
            }
        }
    }
    if (RHINO_SUCCESS != krhino_mutex_unlock(&i2cTransceiver->I2CMutexLock))
    {
        retcode = RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_SEMAPHORE_ERROR);
    }

    return retcode;
}

/* API documentation is in the interface header file */
Retcode_T I2CTransceiver_Write(I2cTranceiverHandlePtr_T i2cTransceiver, uint8_t I2Caddr, uint8_t Reg, uint8_t *RegData, uint8_t NbrBytesToWrite)
{
    Retcode_T retcode = RETCODE_OK;
    if (UINT8_C(0) == NbrBytesToWrite)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    }

//    if ((NULL == i2cTransceiver) || (NULL == RegData) || (NULL == i2cTransceiver->I2CBusSync) || (NULL == i2cTransceiver->I2CMutexLock) || (NULL == i2cTransceiver->I2CHandle))
//    {
//        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
//    }

    if (false == i2cTransceiver->InitializationStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED));
    }
    if (RHINO_SUCCESS != krhino_mutex_lock(&i2cTransceiver->I2CMutexLock, /*(TickType_t) pdMS_TO_TICKS*/(DATA_TRANSFER_TIMEOUT_MS)))
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR));
    }
    retcode = MCU_I2C_WriteRegister(i2cTransceiver->I2CHandle, (uint16_t) I2Caddr, Reg, RegData, NbrBytesToWrite);
    if (RETCODE_OK == retcode)
    {
        if (RHINO_SUCCESS != krhino_sem_take(&i2cTransceiver->I2CBusSync, /*(TickType_t) pdMS_TO_TICKS*/(DATA_TRANSFER_TIMEOUT_MS)))
        {
            /* Since the I2C transfer time out happened, Abort an ongoing I2C transmission.*/
            retcode = MCU_I2C_Send(i2cTransceiver->I2CHandle, (uint16_t) I2Caddr, RegData, CANCEL_I2C_TRANSMISSION);
            if (RETCODE_OK == retcode)
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIMEOUT);
            }
        }
        else
        {
            if (INT8_C(-1) == i2cTransceiver->I2cTransferStatusFlag)
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_I2CTRANSCEIVER_TRANSFER_ERROR);
            }
        }
    }
    if (RHINO_SUCCESS != krhino_mutex_unlock(&i2cTransceiver->I2CMutexLock))
    {
        retcode = RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_SEMAPHORE_ERROR);
    }


    return retcode;
}

/* API documentation is in the interface header file */
Retcode_T I2CTransceiver_Deinit(I2cTranceiverHandlePtr_T i2cTransceiver)
{
    Retcode_T retcode = RETCODE_OK;

    /*
     if ((NULL == i2cTransceiver) || (NULL == i2cTransceiver->I2CBusSync) || (NULL == i2cTransceiver->I2CMutexLock) || (NULL == i2cTransceiver->I2CHandle))
     {  Handle is not initialized
     return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
     }
     */
    if (false == i2cTransceiver->InitializationStatus)
    {
        return (RETCODE_OK);
    }

    i2cTransceiver->I2CHandle = NULL;
    krhino_sem_del(&i2cTransceiver->I2CBusSync);
//    i2cTransceiver->I2CBusSync = NULL;
    krhino_mutex_del(&i2cTransceiver->I2CMutexLock);
//    i2cTransceiver->I2CMutexLock = NULL;
    i2cTransceiver->InitializationStatus = false;
    return retcode;
}

#endif /* BCDS_FEATURE_I2C */

#endif /* if BCDS_FEATURE_I2CTRANSCEIVER */
