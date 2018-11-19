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

#include "BCDS_PeripheralDriver.h"
#include "BCDS_I2cDriver.h"

#include "I2cDriver.h"

/* Put the type and macro definitions here */

/* Put constant and variable definitions here */

/* Put private function declarations here */

/* Put function implementations here */

Retcode_T I2cDriver_Initialize(I2cDriver_Handle_T i2c, I2cDriver_InterfaceHandle_T interface, I2cDriver_ConfigHandle_T config)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = PeripheralDriver_Initialize((PeripheralDriver_Handle_T) i2c, (PeripheralDriver_InterfaceHandle_T) interface);
    if (RETCODE_OK == retcode)
    {
        retcode = I2cDriver_initialize(i2c, interface, config);
    }
    return retcode;
}

Retcode_T I2cDriver_Open(I2cDriver_Handle_T i2c, I2cDriver_ConfigHandle_T config, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    retcode = PeripheralDriver_Open((PeripheralDriver_Handle_T) i2c, timeout);
    if (RETCODE_OK == retcode)
    {
        retcode = I2cDriver_open(i2c, config);

    }
    return retcode;

}

Retcode_T I2cDriver_Close(I2cDriver_Handle_T i2c)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = I2cDriver_close(i2c);

    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_Close((PeripheralDriver_Handle_T) i2c);
    }

    return retcode;

}

Retcode_T I2cDriver_Write(I2cDriver_Handle_T i2c, uint16_t deviceAddress, void * data, uint32_t length, uint32_t timeout, bool xferPending)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = I2cDriver_write(i2c, deviceAddress, data, length, timeout, xferPending);
    return retcode;
}

Retcode_T I2cDriver_Read(I2cDriver_Handle_T i2c, uint16_t deviceAddress, void * data, uint32_t length, uint32_t timeout, bool xferPending)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = I2cDriver_read(i2c, deviceAddress, data, length, timeout, xferPending);
    return retcode;
}

Retcode_T I2cDriver_Deinitialize(I2cDriver_Handle_T i2c, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = PeripheralDriver_Deinitialize((PeripheralDriver_Handle_T) i2c, timeout);
    if (RETCODE_OK == retcode)
    {
        retcode = I2cDriver_deinitialize(i2c);
    }
    return retcode;
}

