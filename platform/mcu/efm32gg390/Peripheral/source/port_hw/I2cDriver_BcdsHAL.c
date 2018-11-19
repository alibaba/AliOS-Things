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

#include "BCDS_Basics.h"
#include "BCDS_Retcode.h"

#include "BCDS_I2cDriver.h"
#include "BCDS_PeripheralDriver.h"

#include "../I2cDriver.h"

#include "BCDS_MCU_I2C.h"

/* Put the type and macro definitions here */

/* Put constant and variable definitions here */

/* Put private function declarations here */

static void I2CDriver_Callback(I2C_T i2c, struct MCU_I2C_Event_S event);

/* Put function implementations here */

Retcode_T I2cDriver_initialize(I2cDriver_Handle_T i2c, I2cDriver_InterfaceHandle_T interface, I2cDriver_ConfigHandle_T config)
{
    PERIPHERALDRIVER_NULLCHECK_RETURN(i2c);
    PERIPHERALDRIVER_NULLCHECK_RETURN(interface);
    BCDS_UNUSED(config);

    Retcode_T retcode = RETCODE_OK;

    retcode = MCU_I2C_Initialize((I2C_T) interface, I2CDriver_Callback);
    if (RETCODE_OK == retcode)
    {
        retcode = MCU_I2C_SetOwner((I2C_T) interface, (I2C_Owner_T) i2c);
    }

    return retcode;
}

static void I2CDriver_Callback(I2C_T i2c, struct MCU_I2C_Event_S event)
{
    Retcode_T retcode = RETCODE_OK;
    I2C_Owner_T * i2cOwner = NULL;
    I2cDriver_Handle_T i2cDriverHandle = NULL;
    union MCU_I2C_Event_U eventU;

    i2cOwner = MCU_I2C_GetOwner(i2c);
    if (RETCODE_OK == retcode)
    {
        i2cDriverHandle = (I2cDriver_Handle_T) (i2cOwner);
        eventU.bits = event;
        retcode = PeripheralDriver_Signal((PeripheralDriver_Handle_T) i2cDriverHandle, (uint32_t) eventU.value);
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }

    if (RETCODE_OK != retcode)
    {
        Retcode_RaiseError(retcode);
    }

}

static Retcode_T I2cDriver_handleEvent(uint32_t event)
{
    /* @todo - Implement this */

    (void) event;

    return RETCODE_OK;
}

Retcode_T I2cDriver_open(I2cDriver_Handle_T i2c, I2cDriver_ConfigHandle_T config)
{
    /* @todo: need to implement a set config for MCU_I2C in order to set the config on open */
    BCDS_UNUSED(i2c);
    BCDS_UNUSED(config);
    Retcode_T retcode = RETCODE_OK;

    return retcode;
}

Retcode_T I2cDriver_close(I2cDriver_Handle_T i2c)
{
    /* we do not do anything here, as we do nothing special on close today.
     * in the future we may want to implement some enable, disable, power down mechanism here.
     */
    BCDS_UNUSED(i2c);
    Retcode_T retcode = RETCODE_OK;

    return retcode;
}

Retcode_T I2cDriver_write(I2cDriver_Handle_T i2c, uint16_t slaveAddress, void * data, uint32_t length, uint32_t timeout, bool xferPending)
{
    BCDS_UNUSED(timeout);
    Retcode_T retcode = RETCODE_OK;
    assert(i2c!=NULL);
    assert(data!=NULL);
    uint32_t event = 0;

    I2cDriverPtr_T pI2c = (I2cDriverPtr_T) i2c;

    I2C_T i2cInterface = (I2C_T) pI2c->peripheral.Interface;

    retcode = MCU_I2C_Send(i2cInterface, slaveAddress, (uint8_t *) data, length);
    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_WaitSignal(i2c, &event, timeout);
    }

    if (RETCODE_OK == retcode)
    {
        retcode = I2cDriver_handleEvent(event);
    }

    return retcode;
}

Retcode_T I2cDriver_read(I2cDriver_Handle_T i2c, uint16_t slaveAddress, void * data, uint32_t length, uint32_t timeout, bool xferPending)
{
    BCDS_UNUSED(timeout);
    Retcode_T retcode = RETCODE_OK;
    assert(i2c!=NULL);
    assert(data!=NULL);
    uint32_t event = 0;

    I2cDriverPtr_T pI2c = (I2cDriverPtr_T) i2c;

    I2C_T i2cInterface = (I2C_T) pI2c->peripheral.Interface;

    retcode = MCU_I2C_Receive(i2cInterface, slaveAddress, (uint8_t *) data, length);
    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_WaitSignal(i2c, &event, timeout);
    }

    if (RETCODE_OK == retcode)
    {
        retcode = I2cDriver_handleEvent(event);
    }
    return retcode;
}

