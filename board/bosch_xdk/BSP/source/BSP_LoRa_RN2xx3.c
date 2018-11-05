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
 * @brief  Implementation of LoRa/HSPA cellular BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_LORA_RN2XX3

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_LORA_RN2XX3

#include "BSP_BoardType.h"
#include "BSP_BoardExtSettings.h"
#include "BSP_BoardShared.h"
#include "BSP_LoRa_RN2xx3.h"
#include "BCDS_BSP_LoRa_RN2xx3.h"
#include "BCDS_MCU_UART_Handle.h"
#include "BSP_ExtensionPort.h"

#include "em_usart.h"

/*
 * This API implements the synchronization mechanism required for the LORA module UART communication
 */
static void BreakAndSync(uint32_t baudrate)
{
    /* we do the following:
     *  - set baudrate to 300 baud, which in our case will be the lowest possible baudrate that will be configured
     *  - send a break using TXDATAX register
     *  - switch baudrate to desired baudrate
     *  - send a 0x55 for auto baudrate detection
     */
    uint32_t status = UINT32_C(0);
    /* manually configure the UART */
    CMU_ClockEnable(cmuClock_UART1, BSP_ENABLE);
    USART_BaudrateAsyncSet(LORA_SERIAL_PORT, 0, LORA_BREAK_BAUDRATE, USART_CTRL_OVS_X16);
    LORA_SERIAL_PORT->ROUTE |= _USART_ROUTE_TXPEN_MASK;
    LORA_SERIAL_PORT->CMD = _USART_CMD_TXEN_MASK;
    /* we will use the break feature of the EFM32 uart */
    USART_TxExt(LORA_SERIAL_PORT, USART_TXDATAX_TXBREAK);
    do
    {
        status = USART_StatusGet(LORA_SERIAL_PORT);
    } while (!(status & USART_STATUS_TXC));
    /* now we set the desired baudrate and send a sync char*/
    USART_BaudrateAsyncSet(LORA_SERIAL_PORT, 0, baudrate, USART_CTRL_OVS_X16);
    USART_Tx(LORA_SERIAL_PORT, LORA_BREAK_SYNC_CHAR);
    do
    {
        status = USART_StatusGet(LORA_SERIAL_PORT);
    } while (!(status & USART_STATUS_TXC));
    CMU_ClockEnable(cmuClock_UART1, BSP_DISABLE);
}

Retcode_T BSP_LoRa_RN2xx3_Connect(void)
{
    Retcode_T retVal = RETCODE_OK;
    GPIO_DriveModeSet(gpioPortB, gpioDriveModeHigh);
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectUart();
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectGpio(LORA_RTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectGpio(LORA_CTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_ConnectGpio(LORA_RESET_PIN);
    }

    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_LoRa_RN2xx3_Enable(void)
{
    Retcode_T retVal = RETCODE_OK;

    Board_WakeupPowerSupply2V5(EXTENSION_BOARD);
    Board_Delay(LORA_STARTUP_DELAY_MS);

    retVal = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_BAUDRATE, LORA_BAUDRATE, NULL);
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

    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(LORA_RTS_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_PUSHPULL, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(LORA_RTS_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_LOW, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(LORA_RTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(LORA_CTS_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_INPUT_NOPULL, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(LORA_CTS_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_LOW, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(LORA_CTS_PIN);
    }

    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(LORA_RESET_PIN, BSP_EXTENSIONPORT_GPIO_PINMODE, BSP_EXTENSIONPORT_PUSHPULL_DRIVE, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_SetGpioConfig(LORA_RESET_PIN, BSP_EXTENSIONPORT_GPIO_PINVALUE, BSP_EXTENSIONPORT_GPIO_PIN_HIGH, NULL);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_EnableGpio(LORA_RESET_PIN);
    }

    if (RETCODE_OK == retVal)
    {
        retVal = BSP_LoRa_RN2xx3_SetResetPin(0);
        Board_Delay(200);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_LoRa_RN2xx3_SetResetPin(1);
        Board_Delay(200);
    }

    BSP_LoRa_RN2xx3_ChangeBaudrate(LORA_BAUDRATE);

    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_LoRa_RN2xx3_Disable(void)
{
    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_DisableUart();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(LORA_RTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(LORA_CTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisableGpio(LORA_RESET_PIN);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_LoRa_RN2xx3_Disconnect(void)
{

    Retcode_T retVal = RETCODE_OK;
    retVal = BSP_ExtensionPort_DisonnectUart();
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(LORA_RTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(LORA_CTS_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        retVal = BSP_ExtensionPort_DisconnectGpio(LORA_RESET_PIN);
    }
    if (RETCODE_OK == retVal)
    {
        Board_SnoozePowerSupply2V5(EXTENSION_BOARD);
    }
    return retVal;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_LoRa_RN2xx3_SetResetPin(uint32_t state)
{
    Retcode_T retVal = RETCODE_OK;

    if (0 == state)
    {
        retVal = BSP_ExtensionPort_ClearGpio(LORA_RESET_PIN);
    }
    else
    {
        retVal = BSP_ExtensionPort_SetGpio(LORA_RESET_PIN);
    }

    return retVal;
}

/*
 * Refer to interface for description
 */
Retcode_T BSP_LoRa_RN2xx3_ChangeBaudrate(uint32_t baudrate)
{
    /* to be clean, disable the UART */
    BSP_ExtensionPort_DisableUart();
    BreakAndSync(baudrate);
    /* here we go and set the baudrate in terms of HAL/BSP */
    BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_BAUDRATE, baudrate, NULL);
    BSP_ExtensionPort_EnableUart();
    return RETCODE_OK;
}

/*
 * Refer to interface for description
 */
Retcode_T BSP_LoRa_RN2xx3_WakeUp(void)
{
    /*Sync is performed with LORA default baudrate */
    BreakAndSync(LORA_BAUDRATE);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
HWHandle_T BSP_LoRa_RN2xx3_GetUartHandle(void)
{
    HWHandle_T Handle = BSP_ExtensionPort_GetUartHandle();

    return (Handle);
}

#endif /* BCDS_FEATURE_BSP_LoRa_RN2xx3 */

