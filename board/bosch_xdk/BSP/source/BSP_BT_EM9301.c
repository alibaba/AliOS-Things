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
 * @brief  Implementation of Bluetooth BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_BT_EM9301

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_BT_EM9301

#include "BSP_BoardType.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BSP_BT_EM9301.h"
#include "BCDS_MCU_UART_Handle.h"
#include "BCDS_BSP_BT_EM9301.h"

/*
 * enumeration of custom return codes
 */
enum BSP_BT_EM9301_Retcode_E
{
    RETCODE_BSP_BT_SERIAL_NOT_INITIALIZED = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_BSP_BT_DISABLED,
};

static BSPLock_T BT_GPIOLock = BSP_LOCKED;
static UART_IRQ_Callback_T BT_EM9301_UART_RxISRCallback;
static UART_IRQ_Callback_T BT_EM9301_UART_TxISRCallback;

struct MCU_UART_Handle_S BSP_BT_EM9301_UART_Handle;

Retcode_T BSP_BT_EM9301_Connect(void)
{
    GPIO_PinModeSet(BT_IRQ_PORT, BT_IRQ_PIN, BT_IRQ_MODE, 0);
    GPIO_PinModeSet(BT_RST_PORT, BT_RST_PIN, BT_RST_MODE, 0);
    GPIO_PinModeSet(BT_UART0_RX_PORT, BT_UART0_RX_PIN, BT_UART0_RX_MODE, 0);
    GPIO_PinModeSet(BT_UART0_TX_PORT, BT_UART0_TX_PIN, BT_UART0_TX_MODE, 1);
    GPIO_PinModeSet(BT_WAKEUP_PORT, BT_WAKEUP_PIN, BT_WAKEUP_MODE, 0);

    CMU_ClockEnable(BT_UART0_CLOCK, true);
    BT_SERIAL_PORT->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC1;
    CMU_ClockEnable(BT_UART0_CLOCK, false);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_BT_EM9301_Enable(void)
{
    Retcode_T retcode = RETCODE_OK;
    /* Dont enable if the callback has not been registered */
    if (NULL == BSP_BT_EM9301_UART_Handle.IrqRxCallback || NULL == BSP_BT_EM9301_UART_Handle.IrqTxCallback)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_BT_SERIAL_NOT_INITIALIZED);
    }
    /* Enable 2v5 power rail if not already enabled */
    Board_WakeupPowerSupply2V5(BT_EM9301);
    if (RETCODE_OK == retcode)
    {
        /*
         * Wait for the BT module to start up
         */
        Board_Delay(BT_STARTUP_DELAY_MS);
        /*
         * Enable the USART MCU resource
         */
        CMU_ClockEnable(BT_UART0_CLOCK, true);
        /*
         * Clear Pending Interrupt flags
         */
        /* clear interrupt flags */
        BT_SERIAL_PORT->IFC = _USART_IF_MASK;
        /*
         * Enable receive interrupt flags
         */
        /* Enable the UART RX Not Empty Interrupt */
        BT_SERIAL_PORT->IEN = USART_IEN_RXFULL | USART_IEN_RXOF | USART_IEN_FERR;
        /*
         * Associate the ISR callbacks
         */
        BT_EM9301_UART_RxISRCallback = BSP_BT_EM9301_UART_Handle.IrqRxCallback;
        BT_EM9301_UART_TxISRCallback = BSP_BT_EM9301_UART_Handle.IrqTxCallback;

        /*
         * Set the priority for the UART interface
         */
        NVIC_SetPriority(BT_UART_RX_IRQn, BT_UART_RX_PRIORITY);
        NVIC_SetPriority(BT_UART_TX_IRQn, BT_UART_TX_PRIORITY);
        /*
         * Clear Pending interrupt requests
         */
        NVIC_ClearPendingIRQ(BT_UART_RX_IRQn);
        NVIC_ClearPendingIRQ(BT_UART_TX_IRQn);
        /*
         * Enable the interrupt requests
         */
        NVIC_EnableIRQ(BT_UART_RX_IRQn);
        NVIC_EnableIRQ(BT_UART_TX_IRQn);
        /* Unlock the bluetooth module GPIOs */
        BT_GPIOLock = BSP_UNLOCKED;
        /* Enable the UART */
        /*Enable transmitter and receiver*/
        USART_Enable(UART0, (USART_Enable_TypeDef) (USART_CMD_RXEN | USART_CMD_TXEN));
        /*
         * Check if the USART has been enabled
         */
        Board_Delay(1);
        if (((BT_SERIAL_PORT->STATUS & _USART_STATUS_RXENS_MASK) == 0) || ((BT_SERIAL_PORT->STATUS & _USART_STATUS_TXENS_MASK) == 0))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
        }
    }
    return retcode;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_BT_EM9301_Disable(void)
{
    Retcode_T retcode = RETCODE_OK;
    /* Disable USART interface */
    BT_SERIAL_PORT->CMD = USART_CMD_RXDIS | USART_CMD_TXDIS | USART_CMD_MASTERDIS;
    /*
     * Check if the USART has been disabled
     */
    if (((BT_SERIAL_PORT->STATUS & _USART_STATUS_RXENS_MASK) != 0) || ((BT_SERIAL_PORT->STATUS & _USART_STATUS_TXENS_MASK) != 0))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    if (RETCODE_OK == retcode)
    {
        BT_GPIOLock = BSP_LOCKED;
        NVIC_DisableIRQ(BT_UART_RX_IRQn);
        NVIC_DisableIRQ(BT_UART_TX_IRQn);
        BT_EM9301_UART_RxISRCallback = NULL;
        BT_EM9301_UART_TxISRCallback = NULL;
        /* @todo to check whether this snooze is required or not and added reset instead snooze to
         * reset the BLE as there is no power control pin*/
        /* Board_SnoozePowerSupply2V5(BT_EM9301);*/
        retcode = BSP_BT_EM9301_Reset();
    }
    return retcode;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_BT_EM9301_Disconnect(void)
{
    GPIO_PinModeSet(BT_IRQ_PORT, BT_IRQ_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BT_RST_PORT, BT_RST_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BT_UART0_RX_PORT, BT_UART0_RX_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BT_UART0_TX_PORT, BT_UART0_TX_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(BT_WAKEUP_PORT, BT_WAKEUP_PIN, gpioModeDisabled, 0);
    return RETCODE_OK;
}

Retcode_T BSP_BT_EM9301_SetWUHigh(void)
{
    Retcode_T retcode = RETCODE_OK;
    if (BSP_UNLOCKED == BT_GPIOLock)
    {
        GPIO_PinOutSet(BT_WAKEUP_PORT, BT_WAKEUP_PIN);
        Board_Delay(BT_WAKEUP_DELAY_MS);
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_BT_DISABLED);
    }
    return retcode;
}

Retcode_T BSP_BT_EM9301_SetWULow(void)
{
    Retcode_T retcode = RETCODE_OK;
    if (BSP_UNLOCKED == BT_GPIOLock)
    {
        GPIO_PinOutClear(BT_WAKEUP_PORT, BT_WAKEUP_PIN);
    }
    else
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_BT_DISABLED);
    }
    return retcode;
}

Retcode_T BSP_BT_EM9301_Reset(void)
{
    GPIO_PinOutSet(BT_RST_PORT, BT_RST_PIN);
    BSP_Board_Delay(BT_RESET_PULLUP_DELAY_MS);
    GPIO_PinOutClear(BT_RST_PORT, BT_RST_PIN);
    BSP_Board_Delay(BT_RESET_PROCESSING_DELAY_MS);
    return RETCODE_OK;

}

/*
 * Refer to interface header for description
 */
HWHandle_T BSP_BT_EM9301_GetUARTHandle(void)
{
    /*
     * Initialize the Handle and return it back to the caller
     */
    BSP_BT_EM9301_UART_Handle.Uart_ptr = UART0;
    BSP_BT_EM9301_UART_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;

    return (HWHandle_T) &BSP_BT_EM9301_UART_Handle;
}

/*
 * Refer to interface header for description
 */
void BT_EM9301_UART_TxISR(void)
{
    if (NULL != BT_EM9301_UART_TxISRCallback)
    {
        /* All UART interrupt processing is to be done at the MCU level */
        BT_EM9301_UART_TxISRCallback((UART_T) &BSP_BT_EM9301_UART_Handle);
    }
}

/*
 * Refer to interface header for description
 */
void BT_EM9301_UART_RxISR(void)
{
    if (NULL != BT_EM9301_UART_RxISRCallback)
    {
        /* All UART interrupt processing is to be done at the MCU level */
        BT_EM9301_UART_RxISRCallback((UART_T) &BSP_BT_EM9301_UART_Handle);
    }
}

#endif /* BCDS_FEATURE_BSP_BT_EM9301 */

