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
 * @brief  Implementation of Max31865 Interface BSP functions
 */

#include "BCDS_BSP.h"

#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_MAX31865

#include "BCDS_HAL.h"

#if BCDS_FEATURE_BSP_MAX31865

#include "BSP_Max31865.h"
#include "BCDS_BSP_Max31865.h"

#include "BSP_BoardType.h"
#include "BSP_BoardExtSettings.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BCDS_MCU_SPI_Handle.h"
#include "BSP_ExtensionPort.h"

/* Put static implementations here */
/**
 * @brief Sets the configuration and enables the CS pin.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
static Retcode_T BSP_Max31865_EnableCS(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetGpioConfig(MAX31865_CS_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_PUSHPULL, NULL);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(MAX31865_CS_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_HIGH, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(MAX31865_CS_PIN);
    }
    return retVal;
}
/*
 * Refer to interface header for description
 */
HWHandle_T BSP_Max31865_GetSPIHandle(void)
{
    HWHandle_T Handle = BSP_ExtensionPort_GetSpiHandle();
    return Handle;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Max31865_SetCSHigh(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetGpio(MAX31865_CS_PIN);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Max31865_SetCSLow(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_ClearGpio(MAX31865_CS_PIN);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Max31865_Connect(void)
{
    Retcode_T retVal = RETCODE_OK;
    Board_WakeupPowerSupply2V5(EXTENSION_BOARD);
    Board_EnablePowerSupply3V3(EXTENSION_BOARD);
    BSP_Board_Delay(50);
    retVal = BSP_ExtensionPort_ConnectSpi();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectGpio(MAX31865_CS_PIN);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */

Retcode_T BSP_Max31865_Enable(void)
{
    Retcode_T retVal = RETCODE_OK;

    retVal = BSP_ExtensionPort_SetSpiConfig(BSP_EXTENSIONPORT_SPI_BAUDRATE, MAX31865_SPI_BAUDRATE, NULL);
    if (RETCODE_OK == retVal)
    {
        /*Max31865 supports only Mode 1 and Mode 3 */
        retVal = BSP_ExtensionPort_SetSpiConfig(BSP_EXTENSIONPORT_SPI_MODE, BSP_EXTENSIONPORT_SPI_MODE1, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        /* MSB first is only supported */
        retVal = BSP_ExtensionPort_SetSpiConfig(BSP_EXTENSIONPORT_SPI_BIT_ORDER, BSP_EXTENSIONPORT_SPI_MSB_FIRST, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_Max31865_EnableCS();
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableSpi();
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Max31865_Disable(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_DisableSpi();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(MAX31865_CS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_Max31865_SetCSHigh();
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Max31865_Disconnect(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_DisonnectSpi();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(MAX31865_CS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        Board_SnoozePowerSupply2V5(EXTENSION_BOARD);
        Board_DisablePowerSupply3V3(EXTENSION_BOARD);
    }
    return retVal;
}

#endif /* BCDS_FEATURE_BSP_MAX31865 */
