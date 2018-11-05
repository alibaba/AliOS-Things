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
#include "ti_cc26xx/BCDS_MCU_I2C_Handle.h"

#if BCDS_FEATURE_I2C && BCDS_I2C_COUNT
/**
 * @brief Declares the context structure used by MCU I2C driver to handle the
 * I2C requests from the application for a particular I2C interface
 */
#define I2C_WRITE_REGISTER_BUFFER_SIZE 16

static void MCU_I2CCallback(I2C_T i2c);

static Retcode_T I2C_AbortTransferAsync(struct MCU_I2C_Handle_S* i2cHandle);
static Retcode_T I2C_AbortTransferSync(struct MCU_I2C_Handle_S* i2cHandle);

static Retcode_T I2C_SendAsync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t* data, uint32_t length);
static Retcode_T I2C_SendSync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t* data, uint32_t length);

static Retcode_T I2C_ReceiveAsync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t* data, uint32_t length);
static Retcode_T I2C_ReceiveSync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t* data, uint32_t length);

static Retcode_T I2C_ReadRegisterAsync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint32_t length);
static Retcode_T I2C_ReadRegisterSync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint32_t length);

static Retcode_T I2C_WriteRegisterAsync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint32_t length);
static Retcode_T I2C_WriteRegisterSync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint32_t length);

static Retcode_T I2C_ReadWrite(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t* writeBuffer, uint32_t writeCount, uint8_t* readBufffer, uint32_t readCount);

void MCU_I2CCallback(I2C_T i2c)
{
    union MCU_I2C_Event_U event;
    event.value = 0;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if (pI2C->transferStatus)
    {
        event.bits.TxComplete = true;
    }
    else
    {
        event.bits.TransferError = true;
    }

    if (pI2C->state != MCU_I2C_STATE_ABORTING)
    { //ignore callback if application intended to abort
        pI2C->appCallback(i2c, event.bits);
    }
    pI2C->state = MCU_I2C_STATE_READY;
}

/** @brief See public interface function description in BCDS_MCU_I2C.h */
Retcode_T MCU_I2C_Initialize(I2C_T i2c, MCU_I2C_Callback_T callback)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if (NULL == pI2C)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym(613, pI2C) after this point pI2C is ensured to be not NULL */
    if (MCU_I2C_STATE_INIT != pI2C->state)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_DOPPLE_INITIALIZATION);
    }
    if (BCDS_HAL_CALL_MODE_BLOCKING == pI2C->callMode)
    {
        pI2C->transferCallback = NULL;
        pI2C->appCallback = NULL;
        pI2C->readFunc = I2C_ReceiveSync;
        pI2C->writeFunc = I2C_SendSync;
        pI2C->readRegisterFunc = I2C_ReadRegisterSync;
        pI2C->writeRegisterFunc = I2C_WriteRegisterSync;
        pI2C->abortFunc = I2C_AbortTransferSync;
        pI2C->state = MCU_I2C_STATE_READY;
        retcode = RETCODE_OK;
    }
    else if (BCDS_HAL_CALL_MODE_NON_BLOCKING == pI2C->callMode)
    {
        if (NULL != callback)
        {
            pI2C->transferCallback = MCU_I2CCallback;
            pI2C->appCallback = callback;
            pI2C->readFunc = I2C_ReceiveAsync;
            pI2C->writeFunc = I2C_SendAsync;
            pI2C->readRegisterFunc = I2C_ReadRegisterAsync;
            pI2C->writeRegisterFunc = I2C_WriteRegisterAsync;
            pI2C->abortFunc = I2C_AbortTransferAsync;
            pI2C->state = MCU_I2C_STATE_READY;
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
    Retcode_T retcode;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if (NULL == pI2C)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym(613, pI2C) after this point pI2C is ensured to be not NULL */
    if (pI2C->state == MCU_I2C_STATE_READY)
    {
        //do not perform if the state is invalid
        pI2C->transferCallback = NULL;
        pI2C->appCallback = NULL;
        pI2C->state = MCU_I2C_STATE_INIT;
        retcode = RETCODE_OK;
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
    }
    /*lint -esym(613, pI2C) re-enable */
    return retcode;
}

Retcode_T MCU_I2C_Send(I2C_T i2c, uint16_t slaveAddress, uint8_t * data, uint32_t length)
{

    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;

    if ((NULL == pI2C) || (NULL == data) || (UINT16_MAX <= length) || (NULL == pI2C->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym(613, pI2C) after this point pI2C is ensured to be not NULL */

    if (length != 0)
    {
        if (MCU_I2C_STATE_READY == pI2C->state)
        {
            retcode = pI2C->writeFunc(pI2C, slaveAddress, data, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_I2C_STATE_RUNNING == pI2C->state)
        {

            /* application wants to cancel ongoing sending which is signaled by len == 0*/
            retcode = pI2C->abortFunc(pI2C);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym(613, pI2C) re-enable*/
    return retcode;
}

Retcode_T MCU_I2C_Receive(I2C_T i2c, uint16_t slaveAddress, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if ((NULL == pI2C) || (NULL == data) || (UINT16_MAX <= length) || (NULL == pI2C->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pI2C) after this point pI2C is ensured to be not NULL */
    if (length != 0)
    {
        if (MCU_I2C_STATE_READY == pI2C->state)
        {
            retcode = pI2C->readFunc(pI2C, slaveAddress, data, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_I2C_STATE_RUNNING == pI2C->state)
        {

            /* application wants to cancel ongoing sending which is signaled by len == 0*/
            retcode = pI2C->abortFunc(pI2C);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, pI2C) re-enable*/
    return retcode;
}

/** See description in the interface declaration */
Retcode_T MCU_I2C_ReadRegister(I2C_T i2c, uint16_t slaveAddress, uint8_t registerAddress, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if ((NULL == pI2C) || (NULL == data) || (UINT16_MAX <= length) || (NULL == pI2C->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pI2C) after this point pI2C is ensured to be not NULL */
    if (length != 0)
    {
        if (MCU_I2C_STATE_READY == pI2C->state)
        {
            retcode = pI2C->readRegisterFunc(pI2C, slaveAddress, registerAddress, data, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_I2C_STATE_RUNNING == pI2C->state)
        {

            /* application wants to cancel ongoing sending which is signaled by len == 0*/
            retcode = pI2C->abortFunc(pI2C);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, pI2C) re-enable*/
    return retcode;
}

/** See description in the interface declaration */
Retcode_T MCU_I2C_WriteRegister(I2C_T i2c, uint16_t slaveAddress, uint8_t registerAddress, uint8_t * data, uint32_t length)
{
    Retcode_T retcode = RETCODE_OK;
    struct MCU_I2C_Handle_S* pI2C = (struct MCU_I2C_Handle_S*) i2c;
    if ((NULL == pI2C) || (NULL == data) || (I2C_WRITE_REGISTER_BUFFER_SIZE < (length + 1)) || (NULL == pI2C->handle))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    /*lint -esym( 613, pI2C) after this point pI2C is ensured to be not NULL */
    if (length != 0)
    {
        if (MCU_I2C_STATE_READY == pI2C->state)
        {
            retcode = pI2C->writeRegisterFunc(pI2C, slaveAddress, registerAddress, data, length);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    else
    {
        if (MCU_I2C_STATE_RUNNING == pI2C->state)
        {

            /* application wants to cancel ongoing sending which is signaled by len == 0*/
            retcode = pI2C->abortFunc(pI2C);
        }
        else
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INCONSITENT_STATE);
        }
    }
    /*lint +esym( 613, pI2C) re-enable*/
    return retcode;
}

static Retcode_T I2C_SendAsync(struct MCU_I2C_Handle_S* pI2C, uint16_t slaveAddress, uint8_t * data, uint32_t len)
{
    pI2C->state = MCU_I2C_STATE_RUNNING;
    return I2C_ReadWrite(pI2C, slaveAddress, data, len, NULL, 0);
}

static Retcode_T I2C_SendSync(struct MCU_I2C_Handle_S* i2c, uint16_t slaveAddress, uint8_t * data, uint32_t len)
{
    Retcode_T retcode;
    i2c->state = MCU_I2C_STATE_RUNNING;
    retcode = I2C_ReadWrite(i2c, slaveAddress, data, len, NULL, 0);
    i2c->state = MCU_I2C_STATE_READY;
    return retcode;
}
Retcode_T I2C_ReceiveAsync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t * data, uint32_t length)
{
    i2cHandle->state = MCU_I2C_STATE_RUNNING;
    return I2C_ReadWrite(i2cHandle, slaveAddress, NULL, 0, data, length);
}

static Retcode_T I2C_ReceiveSync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t * data, uint32_t length)
{
    Retcode_T retcode;

    i2cHandle->state = MCU_I2C_STATE_RUNNING;
    retcode = I2C_ReadWrite(i2cHandle, slaveAddress, NULL, 0, data, length);
    i2cHandle->state = MCU_I2C_STATE_READY;
    return retcode;
}
static Retcode_T I2C_ReadRegisterAsync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint32_t length)
{
    uint8_t wbuffer = registerAddress;
    i2cHandle->state = MCU_I2C_STATE_RUNNING;
    return I2C_ReadWrite(i2cHandle, slaveAddress, &wbuffer, 1, data, length);
}

static Retcode_T I2C_ReadRegisterSync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint32_t length)
{

    Retcode_T retcode;
    uint8_t wbuffer = registerAddress;

    i2cHandle->state = MCU_I2C_STATE_RUNNING;
    retcode = I2C_ReadWrite(i2cHandle, slaveAddress, &wbuffer, 1, data, length);
    i2cHandle->state = MCU_I2C_STATE_READY;
    return retcode;
}

static Retcode_T I2C_WriteRegisterAsync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint32_t length)
{
    if (I2C_WRITE_REGISTER_BUFFER_SIZE < (length + 1))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
    }
    uint8_t dataToWrite[I2C_WRITE_REGISTER_BUFFER_SIZE];

    /* first byte in buffer is registerAddress */
    dataToWrite[0] = registerAddress;
    /* Copy address and data to local buffer for burst write */
    for (uint32_t i = 1; i <= length; i++)
    {
        dataToWrite[i] = *data++;
    }
    i2cHandle->state = MCU_I2C_STATE_RUNNING;
    return I2C_ReadWrite(i2cHandle, slaveAddress, dataToWrite, length + 1, NULL, 0);
}

static Retcode_T I2C_WriteRegisterSync(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint32_t length)
{
    Retcode_T retcode;
    uint8_t dataToWrite[I2C_WRITE_REGISTER_BUFFER_SIZE];
    /* first byte in buffer is registerAddress */
    dataToWrite[0] = registerAddress;
    /* Copy address and data to local buffer for burst write */
    for (uint32_t i = 1; i <= length; i++)
    {
        dataToWrite[i] = *data++;
    }
    i2cHandle->state = MCU_I2C_STATE_RUNNING;
    retcode = I2C_ReadWrite(i2cHandle, slaveAddress, dataToWrite, length + 1, NULL, 0);
    i2cHandle->state = MCU_I2C_STATE_READY;
    return retcode;
}

static Retcode_T I2C_AbortTransferAsync(struct MCU_I2C_Handle_S* i2cHandle)
{
    /* Stop receiver */
    i2cHandle->state = MCU_I2C_STATE_ABORTING;
    I2C_cancel(i2cHandle->handle);
    return RETCODE_OK;
}

static Retcode_T I2C_AbortTransferSync(struct MCU_I2C_Handle_S* i2cHandle)
{
    BCDS_UNUSED(i2cHandle);
    return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NOT_SUPPORTED);
}

static Retcode_T I2C_ReadWrite(struct MCU_I2C_Handle_S* i2cHandle, uint16_t slaveAddress, uint8_t* writeBuffer, uint32_t writeCount, uint8_t* readBufffer, uint32_t readCount)
{
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);

    i2cHandle->transaction.slaveAddress = slaveAddress;
    i2cHandle->transaction.writeBuf = writeBuffer;
    i2cHandle->transaction.writeCount = writeCount;
    i2cHandle->transaction.readBuf = readBufffer;
    i2cHandle->transaction.readCount = readCount;
    i2cHandle->transaction.arg = NULL;
    i2cHandle->transaction.nextPtr = NULL;
    if (I2C_transfer(i2cHandle->handle, &(i2cHandle->transaction)))
    {
        retcode = RETCODE_OK;
    }
    return retcode;
}
#endif /* BCDS_FEATURE_I2C && BCDS_I2C_COUNT */

