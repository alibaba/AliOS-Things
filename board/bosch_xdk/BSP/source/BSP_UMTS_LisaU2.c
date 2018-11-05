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
 * @brief  Implementation of UMTS/HSPA cellular BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */

#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_UMTS_LISAU2

#include "BCDS_HAL.h"

#if BCDS_FEATURE_BSP_UMTS_LISAU2

#include "BSP_BoardType.h"
#include "BSP_BoardExtSettings.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BSP_UMTS_LisaU2.h"
#include "BCDS_BSP_UMTS_LisaU2.h"
#include "BCDS_MCU_UART_Handle.h"
#include "BSP_ExtensionPort.h"

static BSPLock_T UMTS_GPIOLock = BSP_LOCKED;
/*
 * enumeration of custom return codes
 */

enum BSP_UMTS_LisaU2_Retcode_E
{
    RETCODE_BSP_UMTS_DISABLED = RETCODE_FIRST_CUSTOM_CODE,
};

/*@brief Sets the config for UMTS_LisaU2 and enables UART
 *
 * @retVal RETCODE_OK if the config is set and uart is enabled
 *         error codes in case of failure
 */
static Retcode_T BSP_UMTS_LisaU2_EnableUart(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_BAUDRATE, UMTS_UART_BAUDRATE, NULL);
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

/*@brief Sets the config for gpio pins and enables gpio pins
 *
 * @retVal RETCODE_OK if the config is set and uart is enabled
 *         error codes in case of failure
 */
static Retcode_T BSP_UMTS_LisaU2_EnableGpio(void)
{
    Retcode_T retVal = RETCODE_OK;
    Board_WakeupPowerSupply2V5(EXTENSION_BOARD);
    retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_RTS_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_PUSHPULL, NULL);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_RTS_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_LOW, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(UMTS_RTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_CTS_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_INPUT_NOPULL, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_CTS_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_LOW, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(UMTS_CTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(UMTS_POWER_ON_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_OPEN_DRAIN, NULL);
    }
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

/*
 * Refer to interface header for description
 */
Retcode_T BSP_UMTS_LisaU2_Connect(void)
{
    Retcode_T retVal = RETCODE_OK;
    GPIO_DriveModeSet(gpioPortB, gpioDriveModeHigh);
    retVal = BSP_ExtensionPort_ConnectUart();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectGpio(UMTS_RTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectGpio(UMTS_CTS_PIN);
    }
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

/*
 * Refer to interface header for description
 */
Retcode_T BSP_UMTS_LisaU2_Enable(void)
{
    Retcode_T retVal = RETCODE_OK;
    UMTS_GPIOLock = BSP_UNLOCKED;
    retVal = BSP_UMTS_LisaU2_EnableUart();

    if (RETCODE_OK == retVal)
    {
        retVal = BSP_UMTS_LisaU2_EnableGpio();
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpio(UMTS_RESET_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        BSP_Board_Delay(1);
        retVal = BSP_ExtensionPort_ClearGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        BSP_Board_Delay(1);
        retVal = BSP_ExtensionPort_SetGpio(UMTS_POWER_ON_PIN);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_UMTS_LisaU2_Disable(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetGpio(UMTS_POWER_ON_PIN);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpio(UMTS_RESET_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ClearGpio(UMTS_POWER_ON_PIN);
    }
    if (OStoBSPdelayFunc)
    {
        OStoBSPdelayFunc(UMTS_POWER_OFF_PROCESSING_OS_DELAY_MS);
    }
    else
    {
        BSP_Board_Delay(UMTS_POWER_OFF_PROCESSING_DELAY_MS);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableUart();
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(UMTS_RTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(UMTS_CTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(UMTS_RESET_PIN);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_UMTS_LisaU2_Disconnect(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_DisonnectUart();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(UMTS_RTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(UMTS_CTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(UMTS_POWER_ON_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(UMTS_RESET_PIN);
    }
    Board_SnoozePowerSupply2V5(EXTENSION_BOARD);
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_UMTS_LisaU2_SetRTSHigh(void)
{

    Retcode_T retVal = RETCODE_OK;
    if (BSP_UNLOCKED == UMTS_GPIOLock)
    {
        retVal = BSP_ExtensionPort_SetGpio(UMTS_RTS_PIN);
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_UMTS_DISABLED);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_UMTS_LisaU2_SetRTSLow(void)
{
    Retcode_T retVal = RETCODE_OK;
    if (BSP_UNLOCKED == UMTS_GPIOLock)
    {
        retVal = BSP_ExtensionPort_ClearGpio(UMTS_RTS_PIN);
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_UMTS_DISABLED);
    }
    return retVal;
}

uint32_t BSP_UMTS_LisaU2_GetCTS(void)
{
    Retcode_T retVal = RETCODE_OK;
    BSP_IOState_T status = BSP_XDK_IO_STATE_ERROR;
    uint8_t currentPinVal;
    if (BSP_UNLOCKED == UMTS_GPIOLock)
    {
        retVal = BSP_ExtensionPort_ReadGpio(UMTS_CTS_PIN, &currentPinVal);
        if (RETCODE_OK == retVal)
        {
            if (BSP_IO_PIN_HIGH == currentPinVal)
            {
                status = BSP_XDK_IO_STATE_HIGH;
            }
            else
            {
                status = BSP_XDK_IO_STATE_LOW;
            }
        }
    }
    return (uint32_t) status;

}
/*
 * Refer to interface header for description
 */
Retcode_T BSP_UMTS_LisaU2_Reset(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_SetGpio(UMTS_POWER_ON_PIN);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpio(UMTS_RESET_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        Board_Delay(1);
        retVal = BSP_ExtensionPort_ClearGpio(UMTS_RESET_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        if (OStoBSPdelayFunc)
        {
            OStoBSPdelayFunc(UMTS_RESET_PROCESSING_OS_DELAY_MS);
        }
        else
        {
            BSP_Board_Delay(UMTS_RESET_PROCESSING_DELAY_MS);
        }
        retVal = BSP_ExtensionPort_SetGpio(UMTS_RESET_PIN);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */

HWHandle_T BSP_UMTS_LisaU2_GetUARTHandle(void)
{
    /*
     * Initialize the Handle and return it back to the caller
     */
    HWHandle_T Handle = BSP_ExtensionPort_GetUartHandle();

    return Handle;
}

#endif /* BCDS_FEATURE_BSP_UMTS_LisaU2 */

