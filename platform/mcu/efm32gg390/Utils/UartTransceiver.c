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

#include "BCDS_Utils.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_UTILS_MODULE_ID_UART_TRANSCEIVER

#include "BCDS_UARTTransceiver.h"
#include "k_critical.h"

#if BCDS_FEATURE_UARTTRANSCEIVER

#include "BCDS_Retcode.h"
#include "BCDS_Assert.h"
#include "BCDS_RingBuffer.h"
#include "BCDS_MCU_UART.h"
#include "BCDS_MCU_LEUART.h"
#include <stdlib.h>
/*lint +rw(_to_semi) -d__asm=_to_semi */


#define taskENTER_CRITICAL()  RHINO_CRITICAL_ENTER()
#define taskEXIT_CRITICAL()    RHINO_CRITICAL_EXIT()

#if BCDS_FEATURE_UART || BCDS_FEATURE_LEUART
/*
 * Default End of frame checking function
 * This function shall be substituted during uart transceiver initialization by a user defined function
 */
static bool dummyFrameEndCheckFunc(uint8_t x);

/*
 * Refer to interface for description
 */
Retcode_T UARTTransceiver_Initialize(UARTTransceiver_T *transceiver,
        HWHandle_T handle, uint8_t * rawRxBuffer, uint32_t rawRxBufferSize,
        enum UARTTransceiver_UartType_E type)
{
    Retcode_T retcode = RETCODE_OK;
    if (NULL == transceiver || NULL == handle || NULL == rawRxBuffer || 0 == rawRxBufferSize || UART_TRANSCEIVER_UART_TYPE_NONE == type)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        if (UART_TRANSCEIVER_STATE_RESET == transceiver->State)
        {
            transceiver->handle = handle;
            transceiver->UartType = type;
            RingBuffer_Initialize(&(transceiver->UartRxBufDescr), rawRxBuffer, rawRxBufferSize);
            
            mico_rtos_init_semaphore( &transceiver->TxSemaphore, 1 );
            mico_rtos_init_semaphore( &transceiver->RxSemaphore, 1 );
            transceiver->EndOfFrameCheck = dummyFrameEndCheckFunc;

            if (NULL != transceiver->RxSemaphore && NULL != transceiver->TxSemaphore)
            {
                transceiver->State = UART_TRANSCEIVER_STATE_INITIALIZED;
            }
            else
            {
                retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_DOPPLE_INITIALIZATION);
        }
    }
    return retcode;
}

/*
 * Refer to interface for description
 */
Retcode_T UARTTransceiver_Deinitialize(UARTTransceiver_T *transceiver)
{
    Retcode_T retcode;
    if (NULL == transceiver)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        transceiver->handle = NULL;
        
        mico_rtos_deinit_semaphore( &transceiver->RxSemaphore );
        mico_rtos_deinit_semaphore( &transceiver->TxSemaphore );
        transceiver->State = UART_TRANSCEIVER_STATE_RESET;
        transceiver->Mode = UART_TRANSCEIVER_MODE_NONE;
        retcode = RETCODE_OK;
    }
    return retcode;
}

/*
 * Refer to interface for description
 */
Retcode_T UARTTransceiver_Start(UARTTransceiver_T *transceiver, UARTTransceiver_EndofFrameCheckFunc_T frameEndCheckFunc)
{
    Retcode_T retcode = RETCODE_OK;
    if (NULL == transceiver)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        if (transceiver->State == UART_TRANSCEIVER_STATE_INITIALIZED)
        {
            transceiver->Mode = UART_TRANSCEIVER_MODE_SYNCH;
            RingBuffer_Reset(&(transceiver->UartRxBufDescr));
            if (NULL != frameEndCheckFunc)
            {
                transceiver->EndOfFrameCheck = frameEndCheckFunc;
            }
            else
            {
                transceiver->EndOfFrameCheck = dummyFrameEndCheckFunc;
            }
#if BCDS_FEATURE_UART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_UART)
            {
                retcode = MCU_UART_Receive((UART_T) transceiver->handle, &(transceiver->LastByte), 1);
            }
#elif BCDS_FEATURE_LEUART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_LEUART)
            {
                retcode = MCU_LEUART_Receive((LEUART_T) transceiver->handle, &transceiver->LastByte, 1);
            }
#endif
            if (RETCODE_OK == retcode)
            {
                transceiver->State = UART_TRANSCEIVER_STATE_ACTIVE;
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    return retcode;
}

/*
 *  Refer to interface for description
 */
Retcode_T UARTTransceiver_StartInAsyncMode(UARTTransceiver_T *transceiver,
        UARTTransceiver_EndofFrameCheckFunc_T frameEndCheckFunc,
        UARTransceiver_Callback_T callback)
{
    Retcode_T retcode = RETCODE_OK;
    if ((NULL == transceiver) || (NULL == callback))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        if (transceiver->State == UART_TRANSCEIVER_STATE_INITIALIZED)
        {
            transceiver->callback = callback;
            RingBuffer_Reset(&(transceiver->UartRxBufDescr));

            if (NULL != frameEndCheckFunc)
            {
                transceiver->EndOfFrameCheck = frameEndCheckFunc;
            }
            else
            {
                transceiver->EndOfFrameCheck = dummyFrameEndCheckFunc;
            }
#if BCDS_FEATURE_UART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_UART)
            {
                retcode = MCU_UART_Receive((UART_T) transceiver->handle, &(transceiver->LastByte), 1);
            }
#elif BCDS_FEATURE_LEUART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_LEUART)
            {
                retcode = MCU_LEUART_Receive((LEUART_T) transceiver->handle, &transceiver->LastByte, 1);
            }
#endif
            if (RETCODE_OK == retcode)
            {
                transceiver->Mode = UART_TRANSCEIVER_MODE_ASYNCH;
                transceiver->State = UART_TRANSCEIVER_STATE_ACTIVE;
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    return retcode;
}

/*
 * Refer to interface for description
 */
Retcode_T UARTTransceiver_Stop(UARTTransceiver_T *transceiver)
{
    Retcode_T retcode = RETCODE_OK;
    if (NULL == transceiver)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        if ((transceiver->State == UART_TRANSCEIVER_STATE_ACTIVE) || (transceiver->State == UART_TRANSCEIVER_STATE_SUSPENDED))
        {
            transceiver->EndOfFrameCheck = dummyFrameEndCheckFunc;
#if BCDS_FEATURE_UART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_UART)
            {
                retcode = MCU_UART_Receive((UART_T) transceiver->handle, &(transceiver->LastByte), 0);
            }
#elif BCDS_FEATURE_LEUART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_LEUART)
            {
                retcode = MCU_LEUART_Receive((LEUART_T) transceiver->handle, &(transceiver->LastByte), 0);
            }
#endif
            if (RETCODE_OK == retcode)
            {
                transceiver->State = UART_TRANSCEIVER_STATE_INITIALIZED;
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    return retcode;
}

/*
 * Refer to interface for description
 */
Retcode_T UARTTransceiver_Suspend(UARTTransceiver_T *transceiver)
{
    Retcode_T retcode = RETCODE_OK;
    if (NULL == transceiver)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        if (transceiver->State == UART_TRANSCEIVER_STATE_ACTIVE)
        {
#if BCDS_FEATURE_UART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_UART)
            {
                retcode = MCU_UART_Receive((UART_T) transceiver->handle, &(transceiver->LastByte), 0);
            }
#elif BCDS_FEATURE_LEUART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_LEUART)
            {
                retcode = MCU_LEUART_Receive((LEUART_T) transceiver->handle, &(transceiver->LastByte), 0);
            }
#endif
            if (RETCODE_OK == retcode)
            {
                transceiver->State = UART_TRANSCEIVER_STATE_SUSPENDED;
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    return retcode;
}

/*
 * Refer to interface for description
 */
Retcode_T UARTTransceiver_Resume(UARTTransceiver_T *transceiver)
{
    Retcode_T retcode = RETCODE_OK;
    if (NULL == transceiver)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        if (transceiver->State == UART_TRANSCEIVER_STATE_SUSPENDED)
        {
#if BCDS_FEATURE_UART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_UART)
            {
                retcode = MCU_UART_Receive((UART_T) transceiver->handle, &(transceiver->LastByte), 1);
            }
#elif BCDS_FEATURE_LEUART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_LEUART)
            {
                retcode = MCU_LEUART_Receive((LEUART_T) transceiver->handle, &transceiver->LastByte, 1);
            }
#endif
            if (RETCODE_OK == retcode)
            {
                transceiver->State = UART_TRANSCEIVER_STATE_ACTIVE;
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    return retcode;
}

/*
 * Refer to interface for description
 */
Retcode_T UARTTransceiver_ReadData(UARTTransceiver_T *transceiver, uint8_t *buffer, uint32_t size, uint32_t *length, uint32_t timeout_ms)
{
    Retcode_T retcode = RETCODE_OK;
    if (NULL == transceiver || NULL == buffer || NULL == length || 0 == size)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        if (transceiver->State == UART_TRANSCEIVER_STATE_ACTIVE)
        {
            if (transceiver->Mode == UART_TRANSCEIVER_MODE_SYNCH)
            {
                if (kNoErr != mico_rtos_get_semaphore(transceiver->RxSemaphore, timeout_ms ))
                {
                    /* see Event TxComplete in uartEvents_HAL (HAL ISR user callback) */
                    retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
                }
                else
                {
                    if (RETCODE_SUCCESS != transceiver->errorCode)
                    {
                        retcode = RETCODE(RETCODE_SEVERITY_ERROR, transceiver->errorCode);

                    }
                }
            }
            //taskENTER_CRITICAL();
            *length = RingBuffer_Read(&transceiver->UartRxBufDescr, buffer, size);
            //taskEXIT_CRITICAL();
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    return retcode;
}

/*
 * Refer to interface for description
 */
Retcode_T UARTTransceiver_WriteData(UARTTransceiver_T *transceiver, uint8_t* data, uint32_t length, uint32_t timeout_ms)
{
    Retcode_T retcode = RETCODE_OK;
    if (NULL == transceiver || NULL == data || 0 == length)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    else
    {
        if (UART_TRANSCEIVER_STATE_ACTIVE == transceiver->State)
        {
#if BCDS_FEATURE_UART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_UART)
            {
                retcode = MCU_UART_Send((UART_T) transceiver->handle, data, length);
            }
#elif BCDS_FEATURE_LEUART
            if (transceiver->UartType == UART_TRANSCEIVER_UART_TYPE_LEUART)
            {
                retcode = MCU_LEUART_Send((LEUART_T) transceiver->handle, data, length);
            }
#endif
            if (RETCODE_OK == retcode)
            {
                if (transceiver->Mode == UART_TRANSCEIVER_MODE_ASYNCH)
                {
                    //do nothing here
                }
                else
                {
                    if (kNoErr != mico_rtos_get_semaphore(transceiver->TxSemaphore, timeout_ms))
                    {
                        /* see Event TxComplete in uartEvents_HAL (HAL ISR user callback) */
                        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SEMAPHORE_ERROR);
                    }
                    else
                    {

                    }
                }
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    return retcode;
}

#if BCDS_FEATURE_UART
/*
 * Refer to interface for description
 */
void UARTTransceiver_LoopCallback(UARTTransceiver_T *transceiver, struct MCU_UART_Event_S event)
{
    transceiver->AsyncEvent.registerValue = 0;
    transceiver->errorCode = RETCODE_SUCCESS;

    if (event.RxComplete)
    {
        if (1U == RingBuffer_Write(&(transceiver->UartRxBufDescr), &(transceiver->LastByte), 1UL))
        {
            if (transceiver->EndOfFrameCheck(transceiver->LastByte))
            {
                if (transceiver->Mode == UART_TRANSCEIVER_MODE_ASYNCH)
                {
                    transceiver->AsyncEvent.bitfield.RxComplete = 1;
                }
                else
                {
                    if (kNoErr == mico_rtos_set_semaphore(&transceiver->RxSemaphore))
                    {
                    }
                }
            }
        }
    }
    if (event.TxComplete) /* data has been sent */
    {
        if (transceiver->Mode == UART_TRANSCEIVER_MODE_ASYNCH)
        {
            transceiver->AsyncEvent.bitfield.TxComplete = 1;
        }
        else
        {
            if (kNoErr == mico_rtos_set_semaphore(transceiver->TxSemaphore))
            {
            }
        }
    }
    if (event.RxError)
    {
        if (transceiver->Mode == UART_TRANSCEIVER_MODE_ASYNCH)
        {
            transceiver->AsyncEvent.bitfield.RxError = 1;
        }
        else
        {
            transceiver->errorCode = RETCODE_FAILURE;
            if (kNoErr == mico_rtos_set_semaphore(transceiver->RxSemaphore))
            {
            }
        }
    }
    if (transceiver->AsyncEvent.registerValue)
    {
        transceiver->callback(transceiver->AsyncEvent.bitfield);
    }
}

#elif BCDS_FEATURE_LEUART
/*
 * Refer to interface for description
 */
void UARTTransceiver_LoopCallbackLE(UARTTransceiver_T *transceiver, struct MCU_LEUART_Event_S event)
{
    transceiver->AsyncEvent.registerValue = 0;
    transceiver->errorCode = RETCODE_SUCCESS;

    if (event.RxComplete)
    {
        if (1U == RingBuffer_Write(&(transceiver->UartRxBufDescr), (uint8_t *) &(transceiver->LastByte), 1UL))
        {
            if (transceiver->EndOfFrameCheck(transceiver->LastByte))
            {
                if (transceiver->Mode == UART_TRANSCEIVER_MODE_ASYNCH)
                {
                    transceiver->AsyncEvent.bitfield.RxComplete = 1;
                }
                else
                {
                    if (kNoErr == mico_rtos_set_semaphore(transceiver->RxSemaphore))
                    {
                    }
                }
            }
        }
    }
    if (event.TxComplete) /* data has been sent */
    {
        if (transceiver->Mode == UART_TRANSCEIVER_MODE_ASYNCH)
        {
            transceiver->AsyncEvent.bitfield.TxComplete = 1;
        }
        else
        {
            if (kNoErr == mico_rtos_set_semaphore(transceiver->TxSemaphore))
            {
            }
        }
    }
    if (event.RxError)
    {
        if (transceiver->Mode == UART_TRANSCEIVER_MODE_ASYNCH)
        {
            transceiver->AsyncEvent.bitfield.RxError = 1;
        }
        else
        {
            transceiver->errorCode = RETCODE_FAILURE;
            if (kNoErr == mico_rtos_set_semaphore(transceiver->RxSemaphore))
            {
            }
        }
    }
    if (transceiver->AsyncEvent.registerValue)
    {
        transceiver->callback(transceiver->AsyncEvent.bitfield);
    }
}
#endif /* BCDS_FEATURE_LEUART */

static bool dummyFrameEndCheckFunc(uint8_t x)
{
    BCDS_UNUSED(x);
    return false;
}
#endif /* BCDS_FEATURE_UART || BCDS_FEATURE_LEUART */

#endif /* if BCDS_FEATURE_UARTTRANSCEIVER */
