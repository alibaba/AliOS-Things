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
 * @brief Contains the realization of the MCU I2C interface for EFM32
 */
#include "BCDS_Essentials.h"
/* This is used to define the correct module ID for generation of module
 * error codes
 */
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_ESSENTIALS_MODULE_ID_I2C

#include "BCDS_MCU_I2C.h"
#include "BCDS_MCU_I2C_Handle.h"

#if BCDS_FEATURE_I2C && BCDS_I2C_COUNT
/**
 * @brief Declares the context structure used by MCU I2C driver to handle the
 * I2C requests from the application for a particular I2C interface
 */

void MCU_I2C_IRQ_Callback(I2C_T i2c);
void MCU_I2C_IRQ_RX_Callback(I2C_T i2c);
void MCU_I2C_IRQ_TX_Callback(I2C_T i2c);
void MCU_I2C_DMA_RX_Callback(I2C_T i2c);
void MCU_I2C_DMA_TX_Callback(I2C_T i2c);

static Retcode_T I2C_Abort_Transmission_Async(struct MCU_I2C_Handle_S* const i2cHandle);
static Retcode_T I2C_Send_Async(struct MCU_I2C_Handle_S* const i2cHandle, uint16_t slaveAddr, uint8_t * data, uint32_t len);
static Retcode_T I2C_Receive_Async(struct MCU_I2C_Handle_S* const i2cHandle, uint16_t slaveAddr, uint8_t *rx_buffer, uint32_t rx_xfer_size);

void MCU_I2C_IRQ_Callback(I2C_T i2c)
{
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    union MCU_I2C_Event_U event;

    event.value = 0;
    I2C_TransferReturn_TypeDef status = I2C_Transfer(pI2C->instance);
    switch (status)
    {
    case i2cTransferInProgress:
        // Still busy transferring, so let it.
        break;
    case i2cTransferDone:
        // Transfer has completed
        // Disable interrupt
        I2C_IntDisable(pI2C->instance, _I2C_IEN_RESETVALUE);
        I2C_IntClear(pI2C->instance, _I2C_IFC_MASK);
        event.bits.TxComplete = 1;
        break;
    case i2cTransferNack:
        // A NACK has been received while an ACK was expected. This is usually because the slave did not respond to the address.
        // Disable interrupt
        I2C_IntDisable(pI2C->instance, _I2C_IEN_RESETVALUE);
        I2C_IntClear(pI2C->instance, _I2C_IFC_MASK);
        event.bits.TransferError = 1;
        break;
    default:
        // An error situation has arisen.
        // Disable interrupt
        I2C_IntDisable(pI2C->instance, _I2C_IEN_RESETVALUE);
        I2C_IntClear(pI2C->instance, _I2C_IFC_MASK);
        // return error
        event.bits.TransferError = 1;
        break;
    }
    /* we only call the AppCallback while we have a valid event */
    if (event.value && pI2C->_DriverCtx.AppCallback)
    {
        pI2C->_DriverCtx.AppCallback(i2c, event.bits);
    }

}

void MCU_I2C_DMA_RX_Callback(I2C_T i2c)
{
    BCDS_UNUSED(i2c);
}

void MCU_I2C_DMA_TX_Callback(I2C_T i2c)
{
    BCDS_UNUSED(i2c);
}

/** @brief See public interface function description in BCDS_MCU_I2C.h */
Retcode_T MCU_I2C_Initialize(I2C_T i2c, MCU_I2C_Callback_T callback)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if (NULL == pI2C || NULL == callback)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    /*lint -esym(613, pI2C) after this point pI2C is ensured to be not NULL */
    if (RETCODE_OK == retcode)
    {
        if (true == pI2C->_DriverCtx.Initialized)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_DOPPLE_INITIALIZATION);
        }
    }

    if (RETCODE_OK == retcode)
    {
        if (BCDS_HAL_TRANSFER_MODE_INTERRUPT == pI2C->TransferMode)
        {
            pI2C->IRQCallback = MCU_I2C_IRQ_Callback;
            pI2C->DmaTxCallback = NULL;
            pI2C->DmaRxCallback = NULL;
            pI2C->_DriverCtx.AppCallback = callback;
            pI2C->_DriverCtx.Initialized = true;
            retcode = RETCODE_OK;
        }
        else if (BCDS_HAL_TRANSFER_MODE_DMA == pI2C->TransferMode)
        {
            /* todo implement DMA functionality*/
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
        }
        else
        {
            /* all other modes are currently not supported */
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
        }
    }
    /*lint +esym(613, pI2C) re-enable*/
    return retcode;
}

/** @brief See public interface function description in BCDS_MCU_I2C.h */
Retcode_T MCU_I2C_Deinitialize(I2C_T i2c)
{
    /* Here we are checking if the passed i2c handle is valid and if it
     * is valid we mark our local interface context for that handle as being
     * free again.
     */
    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if (NULL == pI2C)
    {

        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym(613, pI2C) after this point pI2C is ensured to be not NULL */
    if (RETCODE_OK == retcode)
    {
        if (false == pI2C->_DriverCtx.Initialized)
        {
            retcode = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_UNINITIALIZED);
        }
    }
    if (RETCODE_OK == retcode)
    {
        if (i2cTransferInProgress == I2C_Transfer(pI2C->instance))
        {
            retcode = I2C_Abort_Transmission_Async(pI2C);

        }
        pI2C->IRQCallback = NULL;
        pI2C->DmaRxCallback = NULL;
        pI2C->DmaTxCallback = NULL;
        pI2C->_DriverCtx.AppCallback = NULL;
        pI2C->_DriverCtx.Initialized = false;

    }
    /*lint -esym(613, pI2C) re-enable */
    return retcode;
}

/** See description in the interface declaration */
Retcode_T MCU_I2C_SetParentHandle(I2C_T i2c, SWHandle_T parentHandle)
{
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    Retcode_T retcode = RETCODE_OK;
    if ((NULL == pI2C) || (NULL == parentHandle))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    else
    {
        pI2C->ParentHandle = parentHandle;
    }
    return retcode;
}

/** See description in the interface declaration */
SWHandle_T MCU_I2C_GetParentHandle(I2C_T i2c)
{
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    return pI2C->ParentHandle;
}

Retcode_T MCU_I2C_Send(I2C_T i2c, uint16_t slaveAddr, uint8_t * data, uint32_t len)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if (NULL == pI2C)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }

    /*lint -esym(613, pI2C) after this point pI2C is ensured to be not NULL */
    if (RETCODE_OK == retcode)
    {
        if (pI2C->_DriverCtx.Initialized)
        {
            if (len != 0)
            {
                if ( NULL != data && UINT16_MAX >= len)
                {
                    retcode = I2C_Send_Async(pI2C, slaveAddr, data, len);
                }
                else
                {
                    retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                }
            }
            else
            {
                /* application wants to cancel ongoing sending which is signaled by len == 0*/
                retcode = I2C_Abort_Transmission_Async(pI2C);
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
        }
    }
    /*lint +esym(613, pI2C) re-enable*/
    return retcode;
}

Retcode_T MCU_I2C_Receive(I2C_T i2c, uint16_t slaveAddr, uint8_t * buffer, uint32_t len)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if (NULL == pI2C)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pI2C) after this point pI2C is ensured to be not NULL */
    if (RETCODE_OK == retcode)
    {
        if (pI2C->_DriverCtx.Initialized)
        {
            if (len != 0)
            {
                if ( NULL != buffer && UINT16_MAX >= len)
                {
                    retcode = I2C_Receive_Async(pI2C, slaveAddr, buffer, len);
                }
                else
                {
                    retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                }
            }
            else
            {
                /* application wants to cancel ongoing sending which is signaled by len == 0*/
                retcode = I2C_Abort_Transmission_Async(pI2C);
            }
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
        }
    }
    /*lint +esym( 613, pI2C) re-enable*/
    return retcode;
}

Retcode_T MCU_I2C_WriteRegister(I2C_T i2c, uint16_t slaveAddr, uint8_t registerAddr, uint8_t * txdata, uint32_t txLen)
{
    Retcode_T retcode = RETCODE_OK;
    I2C_TransferReturn_TypeDef retval = i2cTransferBusErr;

    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;

    if (NULL == pI2C)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pI2C) after this point pI2C is ensured to be not NULL */
    if (RETCODE_OK == retcode)
    {
        if (false == pI2C->_DriverCtx.Initialized)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
        }
    }

    if (RETCODE_OK == retcode)
    {
        if ((0 == txLen) || (NULL == txdata))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
    }

    if (RETCODE_OK == retcode)
    {
        pI2C->_DriverCtx.I2C_seq.flags = I2C_FLAG_WRITE_WRITE;
        //Store buffer info
        pI2C->_DriverCtx.RegisterAddr = registerAddr;
        pI2C->_DriverCtx.I2C_seq.buf[0].data = &(pI2C->_DriverCtx.RegisterAddr);
        pI2C->_DriverCtx.I2C_seq.buf[0].len = 1;
        pI2C->_DriverCtx.I2C_seq.buf[1].data = txdata;
        pI2C->_DriverCtx.I2C_seq.buf[1].len = (uint16_t) txLen;

        if (slaveAddr > 255)
        {
            pI2C->_DriverCtx.I2C_seq.flags |= I2C_FLAG_10BIT_ADDR;
        }
        //slaveAddr = slaveAddr << 1;
        pI2C->_DriverCtx.I2C_seq.addr = slaveAddr;

        // Start the Transfer
        retval = (I2C_TransferReturn_TypeDef) I2C_TransferInit(pI2C->instance, &(pI2C->_DriverCtx.I2C_seq));

        if (retval == i2cTransferDone)
        {
            retcode = RETCODE_OK;
        }
        else if (retval != i2cTransferInProgress)
        {
            I2C_IntDisable(pI2C->instance, _I2C_IEN_RESETVALUE);
            I2C_IntClear(pI2C->instance, _I2C_IFC_MASK);
            retcode = RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, pI2C) re-enable*/
    return retcode;
}

Retcode_T MCU_I2C_ReadRegister(I2C_T i2c, uint16_t slaveAddr, uint8_t registerAddr, uint8_t * rxBuffer, uint32_t rxLen)
{

    Retcode_T retcode = RETCODE_OK;
    I2C_TransferReturn_TypeDef retval = i2cTransferBusErr;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;

    if (NULL == pI2C)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pI2C) after this point pI2C is ensured to be not NULL */
    if (RETCODE_OK == retcode)
    {
        if (false == pI2C->_DriverCtx.Initialized)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
        }
    }
    if (RETCODE_OK == retcode)
    {
        if ((0 == rxLen) || (NULL == rxBuffer))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        }
    }
    /* now we can actually start the I2C transfer as we checked all prerequisites */
    if (RETCODE_OK == retcode)
    {

        pI2C->_DriverCtx.I2C_seq.flags = I2C_FLAG_WRITE_READ;
        //Store buffer info
        pI2C->_DriverCtx.RegisterAddr = registerAddr;
        pI2C->_DriverCtx.I2C_seq.buf[0].data = &(pI2C->_DriverCtx.RegisterAddr);
        pI2C->_DriverCtx.I2C_seq.buf[0].len = 1;
        pI2C->_DriverCtx.I2C_seq.buf[1].data = rxBuffer;
        pI2C->_DriverCtx.I2C_seq.buf[1].len = (uint16_t) rxLen;
        if (slaveAddr > 255)
        {
            pI2C->_DriverCtx.I2C_seq.flags |= I2C_FLAG_10BIT_ADDR;
        }
        //slaveAddr = slaveAddr << 1;
        pI2C->_DriverCtx.I2C_seq.addr = slaveAddr;

        // Start the Transfer
        retval = I2C_TransferInit(pI2C->instance, &(pI2C->_DriverCtx.I2C_seq));

        if (retval == i2cTransferDone)
        {
            retcode = RETCODE_OK;
        }
        else if (retval != i2cTransferInProgress)
        {
            I2C_IntDisable(pI2C->instance, _I2C_IEN_RESETVALUE);
            I2C_IntClear(pI2C->instance, _I2C_IFC_MASK);
            retcode = RETCODE(RETCODE_SEVERITY_FATAL, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, pI2C) re-enable */
    return retcode;
}

/*Async Send and Receive Functions */
/**
 * @brief Begin asynchronous TX transfer. The used buffer is specified in the serial object
 *
 * @param[in]       element_ptr
 * @param[in]       tx_buffer pointer
 * @param[in]       tx_xfer_size
 */

static Retcode_T I2C_Send_Async(struct MCU_I2C_Handle_S* const i2cHandle, uint16_t slaveAddr, uint8_t * data, uint32_t len)
{
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_INVALID_PARAM);
    I2C_TransferReturn_TypeDef retval = i2cTransferBusErr;
    /*lint --e{831, 613} null pointers have been check on a upper level */
    i2cHandle->_DriverCtx.I2C_seq.flags = I2C_FLAG_WRITE;
    i2cHandle->_DriverCtx.I2C_seq.buf[0].data = (uint8_t *) data;
    i2cHandle->_DriverCtx.I2C_seq.buf[0].len = (uint32_t) len;
    if (slaveAddr > 255)
    {
        i2cHandle->_DriverCtx.I2C_seq.flags |= I2C_FLAG_10BIT_ADDR;
    }
    //slaveAddr = slaveAddr << 1;
    i2cHandle->_DriverCtx.I2C_seq.addr = slaveAddr;

    // Start the Transfer
    retval = I2C_TransferInit(i2cHandle->instance, &(i2cHandle->_DriverCtx.I2C_seq));

    if (retval != i2cTransferInProgress)
    {
        I2C_IntDisable(i2cHandle->instance, _I2C_IEN_RESETVALUE);
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    rc = RETCODE_OK;
    return rc;
}

/**
 * @brief Begin asynchronous RX transfer. The used buffer is specified in the serial object.
 *
 * @param[in]       element_ptr
 * @param[out]      rx_buffer pointer
 * @param[in]       rx_xfer_size
 */
static Retcode_T I2C_Receive_Async(struct MCU_I2C_Handle_S* const i2cHandle, uint16_t slaveAddr, uint8_t * data, uint32_t len)
{
    // Check that a buffer has indeed been set up
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_WARNING, RETCODE_INVALID_PARAM);
    I2C_TransferReturn_TypeDef retval = i2cTransferBusErr;
    // Check that a buffer has indeed been set up
    /*lint --e{831, 613} null pointers have been check on a upper level */
    i2cHandle->_DriverCtx.I2C_seq.flags = I2C_FLAG_READ;
    //Store buffer info
    i2cHandle->_DriverCtx.I2C_seq.buf[0].data = data;
    i2cHandle->_DriverCtx.I2C_seq.buf[0].len = (uint16_t) len;
    if (slaveAddr > 255)
    {
        i2cHandle->_DriverCtx.I2C_seq.flags |= I2C_FLAG_10BIT_ADDR;
    }
    //slaveAddr = slaveAddr << 1;
    i2cHandle->_DriverCtx.I2C_seq.addr = slaveAddr;

    // Start the Transfer
    retval = I2C_TransferInit(i2cHandle->instance, &(i2cHandle->_DriverCtx.I2C_seq));

    if (retval != i2cTransferInProgress)
    {
        I2C_IntDisable(i2cHandle->instance, _I2C_IEN_RESETVALUE);
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }

    rc = RETCODE_OK;
    return rc;
}

/**
 * @brief   Abort an ongoing transmission.
 *
 * @param[in]       element_ptr
 */
static Retcode_T I2C_Abort_Transmission_Async(struct MCU_I2C_Handle_S* const i2cHandle)
{
    /*lint --e{831, 613} NULL pointers check before calling this function */
    Retcode_T rc = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    /* Stop receiver */
    i2cHandle->instance->CMD |= I2C_CMD_STOP | I2C_CMD_ABORT;
    /* stop interrupting */
    I2C_IntDisable(i2cHandle->instance, _I2C_IEN_RESETVALUE);
    I2C_IntClear(i2cHandle->instance, _I2C_IF_RESETVALUE);
    rc = RETCODE_OK;

    return rc;
}
#endif //-- BCDS_FEATURE_I2C && BCDS_I2C_COUNT
