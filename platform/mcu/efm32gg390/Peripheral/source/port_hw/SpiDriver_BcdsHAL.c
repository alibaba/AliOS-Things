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

#include "BCDS_SpiDriver.h"
#include "BCDS_PeripheralDriver.h"

#include "../SpiDriver.h"

#include "BCDS_MCU_SPI.h"

/* Put the type and macro definitions here */

/* Put constant and variable definitions here */

/* Put private function declarations here */

static Retcode_T SpiDriver_handleEvent(uint32_t event)
{
    /* @todo - Implement this */

    (void) event;

    return RETCODE_OK;
}

static void SPIDriver_Callback(SPI_T spi, struct MCU_SPI_Event_S event);

/* Put function implementations here */

Retcode_T SpiDriver_initialize(SpiDriver_Handle_T spi, SpiDriver_InterfaceHandle_T interface, SpiDriver_ConfigHandle_T config)
{
    PERIPHERALDRIVER_NULLCHECK_RETURN(spi);
    PERIPHERALDRIVER_NULLCHECK_RETURN(interface);
    BCDS_UNUSED(config);

    Retcode_T retcode = RETCODE_OK;

    retcode = MCU_SPI_Initialize((SPI_T) interface, SPIDriver_Callback);
    if (RETCODE_OK == retcode)
    {
        retcode = MCU_SPI_SetOwner((SPI_T) interface, (SPI_Owner_T) spi);
    }
    return retcode;
}

Retcode_T SpiDriver_deinitialize(SpiDriver_Handle_T spi)
{
    PERIPHERALDRIVER_NULLCHECK_RETURN(spi);

    Retcode_T retcode = RETCODE_OK;

    retcode = MCU_SPI_Deinitialize((SPI_T) ((PeripheralDriverPtr_T) spi)->Interface);

    return retcode;
}

static void SPIDriver_Callback(SPI_T spi, struct MCU_SPI_Event_S event)
{
    Retcode_T retcode = RETCODE_OK;
    SPI_Owner_T spiOwner = NULL;
    SpiDriver_Handle_T spiDriverHandle = NULL;
    union MCU_SPI_Event_U eventU;
    spiOwner = MCU_SPI_GetOwner(spi);
    if (NULL != spiOwner)
    {
        spiDriverHandle = (SpiDriver_Handle_T) spiOwner;
        eventU.bitfield = event;
        retcode = PeripheralDriver_Signal((PeripheralDriver_Handle_T) spiDriverHandle, (uint32_t) eventU.registerValue);
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

Retcode_T SpiDriver_open(SpiDriver_Handle_T spi, SpiDriver_ConfigHandle_T config)
{
    /* @todo: need to implement a set config for MCU_SPI in order to set the config on open */
    BCDS_UNUSED(spi);
    BCDS_UNUSED(config);
    Retcode_T retcode = RETCODE_OK;

    return retcode;
}

Retcode_T SpiDriver_close(SpiDriver_Handle_T spi)
{
    /* we do not do anything here, as we do nothing special on close today.
     * in the future we may want to implement some enable, disable, power down mechanism here.
     */
    BCDS_UNUSED(spi);
    Retcode_T retcode = RETCODE_OK;

    return retcode;
}

Retcode_T SpiDriver_write(SpiDriver_Handle_T spi, void * data, uint32_t length, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    assert(spi!=NULL);
    assert(data!=NULL);
    uint32_t event = 0;

    SpiDriverPtr_T pSpi = (SpiDriverPtr_T) spi;

    SPI_T spiInterface = (SPI_T) pSpi->peripheral.Interface;

    retcode = MCU_SPI_Send(spiInterface, (uint8_t *) data, length);
    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_WaitSignal(spi, &event, timeout);
    }

    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriver_handleEvent(event);
    }

    return retcode;
}

Retcode_T SpiDriver_read(SpiDriver_Handle_T spi, void * data, uint32_t length, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    assert(spi!=NULL);
    assert(data!=NULL);
    uint32_t event = 0;

    SpiDriverPtr_T pSpi = (SpiDriverPtr_T) spi;

    SPI_T spiInterface = (SPI_T) pSpi->peripheral.Interface;

    retcode = MCU_SPI_Receive(spiInterface, (uint8_t *) data, length);
    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_WaitSignal(spi, &event, timeout);
    }

    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriver_handleEvent(event);
    }

    return retcode;
}

Retcode_T SpiDriver_transfer(SpiDriver_Handle_T spi, void * dataOut, void * dataIn, uint32_t length, uint32_t timeout)
{
    Retcode_T retcode = RETCODE_OK;
    assert(spi!=NULL);
    assert(dataIn!=NULL);
    assert(dataOut!=NULL);
    uint32_t event = 0;

    SpiDriverPtr_T pSpi = (SpiDriverPtr_T) spi;

    SPI_T spiInterface = (SPI_T) pSpi->peripheral.Interface;

    retcode = MCU_SPI_Transfer(spiInterface, (uint8_t *)dataOut, (uint8_t *)dataIn, length);
    if (RETCODE_OK == retcode)
    {
        retcode = PeripheralDriver_WaitSignal(spi, &event, timeout);
    }

    if (RETCODE_OK == retcode)
    {
        retcode = SpiDriver_handleEvent(event);
    }

    return retcode;

}

