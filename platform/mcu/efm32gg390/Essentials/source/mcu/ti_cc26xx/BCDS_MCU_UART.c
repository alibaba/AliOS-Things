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

/**
 * @file
 * @brief Contains the realization of the MCU UART interface for TI CC2640
 * @note The current implementation supports only blocking mode
 */

#include "BCDS_Essentials.h"
/* This is used to define the correct module ID for generation of module
 * error codes
 */
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_UART

#include "BCDS_MCU_UART.h"
#include "ti_cc26xx/BCDS_MCU_UART_Handle.h"
#include "drivers/UART.h"

#if BCDS_FEATURE_UART && BCDS_UART_COUNT
/**
 * @brief Declares the context structure used by MCU UART driver to handle the
 * UART requests from the application for a particular UART interface
 */

static void MCU_UARTRxCallback(UART_T uart);
static void MCU_UARTTxCallback(UART_T uart);

static Retcode_T UART_AbortRxAsync(struct MCU_UART_Handle_S* uartHandle);
static Retcode_T UART_AbortRxSync(struct MCU_UART_Handle_S* uartHandle);
static Retcode_T UART_AbortTxAsync(struct MCU_UART_Handle_S* uartHandle);
static Retcode_T UART_AbortTxSync(struct MCU_UART_Handle_S* uartHandle);

static Retcode_T UART_SendAsync(struct MCU_UART_Handle_S* uartHandle, uint8_t* data, uint32_t length);
static Retcode_T UART_SendSync(struct MCU_UART_Handle_S* uartHandle, uint8_t* data, uint32_t length);

static Retcode_T UART_ReceiveAsync(struct MCU_UART_Handle_S* uartHandle, uint8_t* data, uint32_t length);
static Retcode_T UART_ReceiveSync(struct MCU_UART_Handle_S* uartHandle, uint8_t* data, uint32_t length);

static void MCU_UARTTxCallback(UART_T uart)
{
    /*
     * This function will be called from BSP after updating the count value
     * see BSP example for counterpart
     */
    struct MCU_UART_Handle_S* uartHandle = (struct MCU_UART_Handle_S*) uart;
    union MCU_UART_Event_U event;
    event.registerValue = 0;

    if (uartHandle->txSize == uartHandle->txCount)
    {
        event.bitfield.TxComplete = 1;
        uartHandle->appCallback(uart, event.bitfield);
    }
    else
    {
        if (uartHandle->txState != MCU_UART_STATE_ABORTING)
        {
            event.bitfield.TxError = 1;
            uartHandle->appCallback(uart, event.bitfield);
        }
    }
    uartHandle->txState = MCU_UART_STATE_READY;
}

static void MCU_UARTRxCallback(UART_T uart)
{
    /*
     * This function will be called from BSP after updating the count value
     * see BSP example for counterpart
     */
    struct MCU_UART_Handle_S* uartHandle = (struct MCU_UART_Handle_S*) uart;
    union MCU_UART_Event_U event;
    event.registerValue = 0;

    if (uartHandle->rxSize == uartHandle->rxCount)
    {
        event.bitfield.RxComplete = 1;
        if (0 != UART_read(uartHandle->handle, uartHandle->rxBuffer, uartHandle->rxSize))
        {
            event.bitfield.RxError = 1;
        }
        uartHandle->appCallback(uart, event.bitfield);
    }
    else
    {
        if (uartHandle->rxState != MCU_UART_STATE_ABORTING)
        {
            // in case of unexpected error cancel and report to application
            event.bitfield.RxError = 1;
            uartHandle->appCallback(uart, event.bitfield);
        }
    }
    uartHandle->rxState = MCU_UART_STATE_READY;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Initialize(UART_T uart, MCU_UART_Callback_T callback)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_UART_Handle_S* uartHandle = (struct MCU_UART_Handle_S*) uart;
    if (NULL == uartHandle)
    { //Check if valid pointer
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    if (uartHandle->txState != MCU_UART_STATE_INIT || uartHandle->rxState != MCU_UART_STATE_INIT)
    {
        //check if valid state
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
    }

    if (BCDS_HAL_CALL_MODE_NON_BLOCKING == uartHandle->callMode)
    {
        if (NULL != callback)
        {
            uartHandle->rxCallback = MCU_UARTRxCallback;
            uartHandle->txCallback = MCU_UARTTxCallback;
            uartHandle->rxFunc = UART_ReceiveAsync;
            uartHandle->txFunc = UART_SendAsync;
            uartHandle->abortRxFunc = UART_AbortRxAsync;
            uartHandle->abortTxFunc = UART_AbortTxAsync;
            uartHandle->appCallback = callback;

            uartHandle->rxState = MCU_UART_STATE_READY;
            uartHandle->txState = MCU_UART_STATE_READY;
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }

    }
    else if (BCDS_HAL_CALL_MODE_BLOCKING == uartHandle->callMode)
    {
        uartHandle->rxCallback = NULL;
        uartHandle->txCallback = NULL;
        uartHandle->rxFunc = UART_ReceiveSync;
        uartHandle->txFunc = UART_SendSync;
        uartHandle->abortRxFunc = UART_AbortRxSync;
        uartHandle->abortTxFunc = UART_AbortTxSync;

        uartHandle->rxState = MCU_UART_STATE_READY;
        uartHandle->txState = MCU_UART_STATE_READY;
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
    }
    return retcode;
}

/** @brief See public interface function description in BCDS_MCU_UART.h */
Retcode_T MCU_UART_Deinitialize(UART_T uart)
{
    /*
     * Resets the variables to their default state and put the interface into UART_STATE_INIT
     */
    if (NULL == uart)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    Retcode_T retcode = RETCODE_OK;
    struct MCU_UART_Handle_S* uartHandle = (struct MCU_UART_Handle_S*) uart;

    if ((uartHandle->rxState == MCU_UART_STATE_READY) && (uartHandle->txState == MCU_UART_STATE_READY))
    {

        uartHandle->rxCallback = NULL;
        uartHandle->txCallback = NULL;
        uartHandle->rxFunc = NULL;
        uartHandle->txFunc = NULL;
        uartHandle->abortRxFunc = NULL;
        uartHandle->abortTxFunc = NULL;
        uartHandle->txCount = 0;
        uartHandle->rxCount = 0;
        uartHandle->txSize = 0;
        uartHandle->rxSize = 0;
        uartHandle->rxBuffer = NULL;

        uartHandle->rxState = MCU_UART_STATE_INIT;
        uartHandle->txState = MCU_UART_STATE_INIT;
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);

    }
    return retcode;
}

Retcode_T MCU_UART_Send(UART_T uart, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_UART_Handle_S* uartHandle = (struct MCU_UART_Handle_S*) uart;

    if ((NULL == uartHandle) || (NULL == data) || (UINT16_MAX <= length) || (NULL == uartHandle->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, uartHandle) after this point uartHandle is ensured to be not NULL */
    if (0 != length)
    {
        if (MCU_UART_STATE_READY == uartHandle->txState)
        {
            retcode = uartHandle->txFunc(uartHandle, data, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_UART_STATE_RUNNING == uartHandle->txState)
        {
            /* application wants to cancel ongoing sending which is signaled by length == 0*/
            retcode = uartHandle->abortTxFunc(uartHandle);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, uartHandle) re-enable*/
    return retcode;
}
Retcode_T MCU_UART_Receive(UART_T uart, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_UART_Handle_S* uartHandle = (struct MCU_UART_Handle_S*) uart;
    if ((NULL == uartHandle) || (NULL == data) || (UINT16_MAX <= length) || (NULL == uartHandle->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, uartHandle) after this point uartHandle is ensured to be not NULL */

    if (length != 0)
    {
        if (MCU_UART_STATE_READY == uartHandle->rxState)
        {
            retcode = uartHandle->rxFunc(uartHandle, data, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_UART_STATE_RUNNING == uartHandle->rxState)
        {
            /* application wants to cancel ongoing sending which is signaled by length == 0*/
            retcode = uartHandle->abortRxFunc(uartHandle);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, uartHandle) re-enable*/
    return retcode;
}

Retcode_T MCU_UART_GetRxCount(UART_T uart, uint32_t* count)
{
    Retcode_T retcode;
    if (NULL == uart || NULL == count)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    struct MCU_UART_Handle_S* uartHandle = (struct MCU_UART_Handle_S*) uart;

    if (uartHandle->rxState != MCU_UART_STATE_INIT)
    {
        *count = uartHandle->rxCount;
        retcode = RETCODE_OK;
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
    }
    return retcode;
}

static Retcode_T UART_SendAsync(struct MCU_UART_Handle_S* uartHandle, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    uartHandle->txState = MCU_UART_STATE_RUNNING;
    uartHandle->txSize = length;
    uartHandle->txCount = 0;
    int32_t writeRet = UART_write(uartHandle->handle, data, length);
    if (writeRet != 0)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    return retcode;
}

static Retcode_T UART_SendSync(struct MCU_UART_Handle_S* uartHandle, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    uartHandle->txState = MCU_UART_STATE_RUNNING;
    int32_t writeRet = UART_write(uartHandle->handle, data, length);
    uartHandle->txState = MCU_UART_STATE_READY;
    if ((uint32_t) writeRet != length)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    return retcode;
}

Retcode_T UART_ReceiveAsync(struct MCU_UART_Handle_S* uartHandle, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    uartHandle->rxState = MCU_UART_STATE_RUNNING;
    uartHandle->rxBuffer = data;
    uartHandle->rxSize = length;
    uartHandle->rxCount = 0;
    int32_t readRet = UART_read(uartHandle->handle, uartHandle->rxBuffer, uartHandle->rxSize);
    if (readRet != 0)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    return retcode;
}

static Retcode_T UART_ReceiveSync(struct MCU_UART_Handle_S* uartHandle, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    uartHandle->rxState = MCU_UART_STATE_RUNNING;
    int32_t readRet = UART_read(uartHandle->handle, data, length);
    uartHandle->rxState = MCU_UART_STATE_READY;
    if ((uint32_t) readRet != length)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    return retcode;
}

static Retcode_T UART_AbortRxAsync(struct MCU_UART_Handle_S* uartHandle)
{
    uartHandle->rxState = MCU_UART_STATE_ABORTING;
    UART_readCancel(uartHandle->handle);
    return RETCODE_OK;
}

static Retcode_T UART_AbortTxAsync(struct MCU_UART_Handle_S* uartHandle)
{
    uartHandle->txState = MCU_UART_STATE_ABORTING;
    UART_writeCancel(uartHandle->handle);
    return RETCODE_OK;
}

static Retcode_T UART_AbortRxSync(struct MCU_UART_Handle_S* uartHandle)
{
    BCDS_UNUSED(uartHandle);
    return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
}

static Retcode_T UART_AbortTxSync(struct MCU_UART_Handle_S* uartHandle)
{
    BCDS_UNUSED(uartHandle);
    return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
}
#endif /* BCDS_FEATURE_UART && BCDS_UART_COUNT */

