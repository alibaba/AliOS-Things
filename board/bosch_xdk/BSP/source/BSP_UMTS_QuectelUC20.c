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
 * @brief  Implementation of UMTS/HSPA Quectel UC20 module BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_UMTS_QUECTELUC20

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_UMTS_QUECTELUC20

#include "BSP_BoardExtSettings.h"
#include "BSP_BoardShared.h"
#include "BCDS_MCU_UART_Handle.h"
#include "BSP_UMTS_QuectelUC20.h"
#include "BSP_ExtensionPort.h"

static Retcode_T BSP_UMTS_QuectelUC20_Connect(void)
{
    Retcode_T retVal = RETCODE_OK;
    GPIO_DriveModeSet(gpioPortB, gpioDriveModeHigh);
    retVal = BSP_ExtensionPort_ConnectUart();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectGpio(UMTS_RESET_PIN);
    }
    return retVal;
}

static Retcode_T BSP_UMTS_QuectelUC20_EnableGpio(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_POWER_ON_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_OPEN_DRAIN, NULL);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_POWER_ON_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_HIGH, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_RESET_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_OPEN_DRAIN, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_RESET_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_HIGH, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(UMTS_RESET_PIN);
    }
    return retVal;
}

static Retcode_T BSP_UMTS_QuectelUC20_EnableUart(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_BAUDRATE, UART1_BAUDRATE, NULL);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_PARITY, BSP_EXTENSIONPORT_UART_NO_PARITY, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_STOPBITS, BSP_EXTENSIONPORT_UART_STOPBITS_ONE, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableUart();
    }
    return retVal;
}

Retcode_T BSP_UMTS_QuectelUC20_Enable(void)
{
    Retcode_T retVal = RETCODE_OK;
    Board_WakeupPowerSupply2V5(EXTENSION_BOARD);
    Board_EnablePowerSupply3V3(EXTENSION_BOARD);
    retVal = BSP_UMTS_QuectelUC20_Connect();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_UMTS_QuectelUC20_EnableGpio();
    }
    OStoBSPdelayFunc(UMTS_GPIO_CONNECT_DELAY_MS);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_UMTS_QuectelUC20_EnableUart();
    }
    return retVal;
}

Retcode_T BSP_UMTS_QuectelUC20_Disable(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_DisableUart();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(UMTS_RESET_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisonnectUart();
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(UMTS_RESET_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        Board_SnoozePowerSupply2V5(EXTENSION_BOARD);
        Board_DisablePowerSupply3V3(EXTENSION_BOARD);
    }
    return retVal;
}

Retcode_T BSP_UMTS_QuectelUC20_PowerOn(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_ClearGpio(UMTS_POWER_ON_PIN);
    if (RETCODE_OK == retVal)
    {
        OStoBSPdelayFunc(UMTS_POWER_ON_PROCESSING_DELAY_MS);
        retVal = BSP_ExtensionPort_SetGpio(UMTS_POWER_ON_PIN);
    }
    return retVal;
}

Retcode_T BSP_UMTS_QuectelUC20_PowerOff(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetGpio(UMTS_POWER_ON_PIN);
    if (RETCODE_OK == retVal)
    {
        OStoBSPdelayFunc(UMTS_POWER_OFF_PROCESSING_DELAY_MS);
        retVal = BSP_ExtensionPort_ClearGpio(UMTS_POWER_ON_PIN);
    }
    return retVal;
}

Retcode_T BSP_UMTS_QuectelUC20_Reset(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_ClearGpio(UMTS_RESET_PIN);
    if (RETCODE_OK == retVal)
    {
        OStoBSPdelayFunc(UMTS_RESET_PROCESSING_DELAY_MS);
        retVal = BSP_ExtensionPort_SetGpio(UMTS_RESET_PIN);
    }
    return retVal;
}

HWHandle_T BSP_UMTS_QuectelUC20_GetUARTHandle(void)
{
    HWHandle_T Handle = BSP_ExtensionPort_GetUartHandle();

    return (Handle);
}

#endif /* BCDS_FEATURE_BSP_UMTS_QuectelUC20 */

