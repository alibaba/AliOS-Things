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
#include "BCDS_SpiDriver.h"

#include "SpiDriver.h"

/* Put the type and macro definitions here */

/* Put constant and variable definitions here */

/* Put private function declarations here */

/* Put function implementations here */

Retcode_T SpiDriver_Initialize(SpiDriver_Handle_T spi, SpiDriver_InterfaceHandle_T interface, SpiDriver_ConfigHandle_T config)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = PeripheralDriver_Initialize((PeripheralDriver_Handle_T) spi, (PeripheralDriver_InterfaceHandle_T) interface);
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriver_initialize(spi, interface, config);
    }
    return retcode;
}

Retcode_T SpiDriver_Deinitialize(SpiDriver_Handle_T spi, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = PeripheralDriver_Deinitialize((PeripheralDriver_Handle_T) spi, timeout);
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriver_deinitialize(spi);
    }
    return retcode;
}

Retcode_T SpiDriver_Open(SpiDriver_Handle_T spi, SpiDriver_ConfigHandle_T config, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    retcode = PeripheralDriver_Open((PeripheralDriver_Handle_T) spi, timeout);
    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriver_open(spi, config);

    }
    return retcode;

}

Retcode_T SpiDriver_Close(SpiDriver_Handle_T spi)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = SpiDriver_close(spi);

    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_Close((PeripheralDriver_Handle_T) spi);
    }

    return retcode;

}

Retcode_T SpiDriver_Write(SpiDriver_Handle_T spi, void * data, uint32_t length, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = SpiDriver_write(spi, data, length, timeout);

    return retcode;

}

Retcode_T SpiDriver_Read(SpiDriver_Handle_T spi, void * data, uint32_t length, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = SpiDriver_read(spi, data, length, timeout);
    return retcode;
}

/**
 *
 */
Retcode_T SpiDriver_Transfer(SpiDriver_Handle_T spi, void * dataOut, void * dataIn, uint32_t length, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;

    retcode = SpiDriver_transfer(spi, dataOut, dataIn, length, timeout);
    return retcode;

}

