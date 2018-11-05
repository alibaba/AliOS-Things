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
 * @brief Contains the realization of the MCU SPI interface for EFM32
 */
#include "BCDS_Essentials.h"
/* This is used to define the correct module ID for generation of module
 * error codes
 */
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_SPI

#include "BCDS_MCU_SPI.h"
#include "ti_cc26xx/BCDS_MCU_SPI_Handle.h"

#if BCDS_FEATURE_SPI && BCDS_SPI_COUNT
/**
 * @brief Declares the context structure used by MCU SPI driver to handle the
 * SPI requests from the application for a particular SPI interface
 */

static void MCU_SPICallback(SPI_T spi);

static Retcode_T SPI_AbortTransferAsync(struct MCU_SPI_Handle_S* spiHandle);
static Retcode_T SPI_AbortTransferSync(struct MCU_SPI_Handle_S* spiHandle);

static Retcode_T SPI_SendAsync(struct MCU_SPI_Handle_S* spiHandle, uint8_t* data, uint32_t length);
static Retcode_T SPI_SendSync(struct MCU_SPI_Handle_S* spiHandle, uint8_t* data, uint32_t length);

static Retcode_T SPI_ReceiveAsync(struct MCU_SPI_Handle_S* spiHandle, uint8_t* data, uint32_t length);
static Retcode_T SPI_ReceiveSync(struct MCU_SPI_Handle_S* spiHandle, uint8_t* data, uint32_t length);

static Retcode_T SPI_TransferAsync(struct MCU_SPI_Handle_S* spiHandle, uint8_t* dataOut, uint8_t* dataIn, uint32_t length);
static Retcode_T SPI_TransferSync(struct MCU_SPI_Handle_S* spiHandle, uint8_t* dataOut, uint8_t* dataIn, uint32_t length);

static inline Retcode_T SPI_SendReceive(struct MCU_SPI_Handle_S* spiHandle, uint8_t* transmitBuffer, uint8_t* receiveBuffer, uint32_t count);

static void MCU_SPICallback(SPI_T spi)
{
    union MCU_SPI_Event_U event;
    event.registerValue = 0;
    struct MCU_SPI_Handle_S* pSPI = (struct MCU_SPI_Handle_S*) spi;

    if (SPI_TRANSFER_COMPLETED == pSPI->transaction.status)
    {
        if (pSPI->state == MCU_SPI_STATE_TX)
        {
            event.bitfield.TxComplete = true;
        }
        else if (pSPI->state == MCU_SPI_STATE_RX)
        {
            event.bitfield.RxComplete = true;
        }
        else if (pSPI->state == MCU_SPI_STATE_TX_RX)
        {
            event.bitfield.RxComplete = true;
            event.bitfield.TxComplete = true;
        }
    }
    else if (SPI_TRANSFER_STARTED != pSPI->transaction.status)
    {
        if (pSPI->state == MCU_SPI_STATE_TX)
        {
            event.bitfield.TxError = true;
        }
        else if (pSPI->state == MCU_SPI_STATE_RX)
        {
            event.bitfield.RxError = true;
            event.bitfield.DataLoss = true;
        }
        else if (pSPI->state == MCU_SPI_STATE_TX_RX)
        {
            event.bitfield.RxError = true;
            event.bitfield.TxError = true;
            event.bitfield.DataLoss = true;
        }
    }
    pSPI->count = pSPI->transaction.count;
    if (MCU_SPI_STATE_ABORTING != pSPI->state)
    { /* ignore application callback if the application intended to cancel a transfer */
        pSPI->appCallback(spi, event.bitfield);
    }
    pSPI->state = MCU_SPI_STATE_READY;
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
Retcode_T MCU_SPI_Initialize(SPI_T spi, MCU_SPI_Callback_T callback)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_SPI_Handle_S* pSPI = (struct MCU_SPI_Handle_S*) spi;
    if (NULL == pSPI)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym(613, pSPI) after this point pSPI is ensured to be not NULL */
    if (MCU_SPI_STATE_INIT != pSPI->state)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_DOPPLE_INITIALIZATION);
    }
    if (BCDS_HAL_CALL_MODE_BLOCKING == pSPI->callMode)
    {
        pSPI->transferCallback = NULL;
        pSPI->appCallback = NULL;
        pSPI->readFunc = SPI_ReceiveSync;
        pSPI->writeFunc = SPI_SendSync;
        pSPI->transferFunc = SPI_TransferSync;
        pSPI->abortFunc = SPI_AbortTransferSync;
        pSPI->state = MCU_SPI_STATE_READY;
        retcode = RETCODE_OK;
    }
    else if (BCDS_HAL_CALL_MODE_NON_BLOCKING == pSPI->callMode)
    {
        if (NULL != callback)
        {
            pSPI->transferCallback = MCU_SPICallback;
            pSPI->appCallback = callback;
            pSPI->readFunc = SPI_ReceiveAsync;
            pSPI->writeFunc = SPI_SendAsync;
            pSPI->transferFunc = SPI_TransferAsync;
            pSPI->abortFunc = SPI_AbortTransferAsync;
            pSPI->state = MCU_SPI_STATE_READY;
            retcode = RETCODE_OK;
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
    }
    else
    {
        /* all other modes are currently not supported */
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
    }
    /*lint +esym(613, pSPI) re-enable*/
    return retcode;
}

/** @brief See public interface function description in BCDS_MCU_SPI.h */
Retcode_T MCU_SPI_Deinitialize(SPI_T spi)
{
    /* Here we are checking if the passed spi handle is valid and if it
     * is valid we mark our local interface context for that handle as being
     * free again.
     */
    Retcode_T retcode;
    struct MCU_SPI_Handle_S* pSPI = (struct MCU_SPI_Handle_S*) spi;
    if (NULL == pSPI)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym(613, pSPI) after this point pSPI is ensured to be not NULL */
    if (MCU_SPI_STATE_READY == pSPI->state)
    { // only perform if the state is valid
        pSPI->transferCallback = NULL;
        pSPI->appCallback = NULL;
        pSPI->count = 0;
        /*reset buffers */
        pSPI->transaction.count = 0;
        pSPI->transaction.rxBuf = NULL;
        pSPI->transaction.txBuf = NULL;
        pSPI->state = MCU_SPI_STATE_INIT;
        retcode = RETCODE_OK;
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
    }

    /*lint -esym(613, pSPI) re-enable */
    return retcode;
}

Retcode_T MCU_SPI_Send(SPI_T spi, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_SPI_Handle_S* pSPI = (struct MCU_SPI_Handle_S*) spi;

    if ((NULL == pSPI) || (NULL == data) || (UINT16_MAX <= length) || (NULL == pSPI->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pSPI) after this point pSPI is ensured to be not NULL */

    if (length != 0)
    {
        if (MCU_SPI_STATE_READY == pSPI->state)
        {
            retcode = pSPI->writeFunc(pSPI, data, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_SPI_STATE_TX == pSPI->state)
        {
            /* application wants to cancel ongoing sending which is signaled by length == 0*/
            retcode = pSPI->abortFunc(pSPI);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, pSPI) re-enable*/
    return retcode;
}

Retcode_T MCU_SPI_Receive(SPI_T spi, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_SPI_Handle_S* pSPI = (struct MCU_SPI_Handle_S*) spi;
    if ((NULL == pSPI) || (NULL == data) || (UINT16_MAX <= length) || (NULL == pSPI->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pSPI) after this point pSPI is ensured to be not NULL */

    if (length != 0)
    {
        if (MCU_SPI_STATE_READY == pSPI->state)
        {
            retcode = pSPI->readFunc(pSPI, data, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_SPI_STATE_RX == pSPI->state)
        {
            /* application wants to cancel ongoing sending which is signaled by length == 0*/
            retcode = pSPI->abortFunc(pSPI);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, pSPI) re-enable*/
    return retcode;
}

Retcode_T MCU_SPI_Transfer(SPI_T spi, uint8_t * dataOut, uint8_t * dataIn, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_SPI_Handle_S* pSPI = (struct MCU_SPI_Handle_S*) spi;
    if ((NULL == pSPI) || (NULL == dataOut) || (NULL == dataIn) || (UINT16_MAX <= length) || (NULL == pSPI->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pSPI) after this point pSPI is ensured to be not NULL */

    if (length != 0)
    {
        if (MCU_SPI_STATE_READY == pSPI->state)
        {
            retcode = pSPI->transferFunc(pSPI, dataOut, dataIn, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_SPI_STATE_TX_RX == pSPI->state)
        {
            /* application wants to cancel ongoing sending which is signaled by length == 0*/
            retcode = pSPI->abortFunc(pSPI);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, pSPI) re-enable*/
    return retcode;
}

uint32_t MCU_SPI_GetDataCount(SPI_T spi)
{
    if (NULL != spi)
    {
        struct MCU_SPI_Handle_S* pSPI = (struct MCU_SPI_Handle_S*) spi;
        return pSPI->count;
    }
    else
    {
        return 0;
    }
}

static Retcode_T SPI_SendAsync(struct MCU_SPI_Handle_S* pSPI, uint8_t * data, uint32_t length)
{
    pSPI->state = MCU_SPI_STATE_TX;
    return SPI_SendReceive(pSPI, data, NULL, length);
}

static Retcode_T SPI_SendSync(struct MCU_SPI_Handle_S* spiHandle, uint8_t * data, uint32_t length)
{
    Retcode_T retcode;

    spiHandle->state = MCU_SPI_STATE_TX;
    retcode = SPI_SendReceive(spiHandle, data, NULL, length);
    spiHandle->count = spiHandle->transaction.count;
    spiHandle->state = MCU_SPI_STATE_READY;
    return retcode;
}
Retcode_T SPI_ReceiveAsync(struct MCU_SPI_Handle_S* spiHandle, uint8_t * data, uint32_t length)
{
    spiHandle->state = MCU_SPI_STATE_RX;
    return SPI_SendReceive(spiHandle, NULL, data, length);
}

static Retcode_T SPI_ReceiveSync(struct MCU_SPI_Handle_S* spiHandle, uint8_t * data, uint32_t length)
{
    Retcode_T retcode;

    spiHandle->state = MCU_SPI_STATE_RX;
    retcode = SPI_SendReceive(spiHandle, NULL, data, length);
    spiHandle->count = spiHandle->transaction.count;
    spiHandle->state = MCU_SPI_STATE_READY;
    return retcode;
}

Retcode_T SPI_TransferAsync(struct MCU_SPI_Handle_S* spiHandle, uint8_t * dataOut, uint8_t * dataIn, uint32_t length)
{
    spiHandle->state = MCU_SPI_STATE_TX_RX;
    return SPI_SendReceive(spiHandle, dataOut, dataIn, length);
}

static Retcode_T SPI_TransferSync(struct MCU_SPI_Handle_S* spiHandle, uint8_t * dataOut, uint8_t * dataIn, uint32_t length)
{
    Retcode_T retcode;

    spiHandle->state = MCU_SPI_STATE_TX_RX;
    retcode = SPI_SendReceive(spiHandle, dataOut, dataIn, length);
    spiHandle->count = spiHandle->transaction.count;
    spiHandle->state = MCU_SPI_STATE_READY;
    return retcode;
}

static Retcode_T SPI_AbortTransferAsync(struct MCU_SPI_Handle_S* spiHandle)
{
    /* Stop receiver */
    spiHandle->state = MCU_SPI_STATE_ABORTING;
    SPI_transferCancel(spiHandle->handle);
    return RETCODE_OK;
}

static Retcode_T SPI_AbortTransferSync(struct MCU_SPI_Handle_S* spiHandle)
{
    BCDS_UNUSED(spiHandle);
    /* if code reaches here then the state of the peripheral is undefined */
    return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
}

static inline Retcode_T SPI_SendReceive(struct MCU_SPI_Handle_S* spiHandle, uint8_t* transmitBuffer, uint8_t* receiveBuffer, uint32_t count)
{
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    spiHandle->transaction.txBuf = transmitBuffer;
    spiHandle->transaction.rxBuf = receiveBuffer;
    spiHandle->transaction.count = count;
    spiHandle->transaction.arg = NULL;
    if (SPI_transfer(spiHandle->handle, &(spiHandle->transaction)))
    {
        retcode = RETCODE_OK;
    }
    return retcode;
}
#endif /* BCDS_FEATURE_SPI && BCDS_SPI_COUNT */

