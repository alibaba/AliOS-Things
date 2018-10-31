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
 * @brief  Implementation of WiFi BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_WIFI_CC3100MOD

#include "BCDS_HAL.h"

#if BCDS_FEATURE_BSP_WIFI_CC3100MOD

#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BSP_WiFi_cc3100mod.h"
#include "BCDS_BSP_WiFi_CC3100MOD.h"
#include "BCDS_MCU_SPI_Handle.h"

/*
 * enumeration of custom return codes
 */
enum BSP_BT_EM9301_Retcode_E
{
    RETCODE_BSP_WIFI_SERIAL_NOT_INITIALIZED = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_BSP_WIFI_DISABLED,
};

struct MCU_SPI_Handle_S BSP_WiFi_CC3100MOD_SPI_Handle;

/*
 * function to handle the interrupts occurring on the GPIO interrupt
 * line of the WiFi module.
 */
static void WiFi_INT_InterruptCallback(void);

static BSP_WiFi_CC3100MOD_InterruptCallback_T WiFi_CC3100MOD_ISRCallback;

#if BCDS_FEATURE_DMA
#include "BSP_DMA.h"
#include "BCDS_MCU_DMA_Handle.h"

struct MCU_DMA_Channel_S BSP_WiFi_CC3100MOD_SPI_TX_Channel;
struct MCU_DMA_Channel_S BSP_WiFi_CC3100MOD_SPI_RX_Channel;
#endif

/*
 * Refer to interface header for description
 */
Retcode_T BSP_WiFi_CC3100MOD_Connect(void)
{
    GPIO_PinModeSet(VDD_WIFI_EN_PORT, VDD_WIFI_EN_PIN, VDD_WIFI_EN_MODE, 1);
    GPIO_PinModeSet(WIFI_NRESET_PORT, WIFI_NRESET_PIN, WIFI_NRESET_MODE, 1);
    GPIO_PinModeSet(WIFI_CSN_PORT, WIFI_CSN_PIN, WIFI_CSN_MODE, 1);
    GPIO_PinModeSet(WIFI_INT_PORT, WIFI_INT_PIN, WIFI_INT_MODE, 0);
    GPIO_PinModeSet(WIFI_NHIB_PORT, WIFI_NHIB_PIN, WIFI_NHIB_MODE, 1);
    GPIO_PinModeSet(WIFI_SPI0_MISO_PORT, WIFI_SPI0_MISO_PIN, WIFI_SPI0_MISO_MODE, 0);
    GPIO_PinModeSet(WIFI_SPI0_MOSI_PORT, WIFI_SPI0_MOSI_PIN, WIFI_SPI0_MOSI_MODE, 0);
    GPIO_PinModeSet(WIFI_SPI0_SCK_PORT, WIFI_SPI0_SCK_PIN, WIFI_SPI0_SCK_MODE, 0);

    /* Set the routing for the WIFI SPI pins TX,RX and clock */
    /* Chip select (CS) pin will be controlled by software */
    CMU_ClockEnable(WIFI_SPI0_CLOCK, true);
    WIFI_SERIAL_PORT->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_LOCATION_LOC0;
    CMU_ClockEnable(WIFI_SPI0_CLOCK, false);

#if BCDS_FEATURE_DMA
    BSP_WiFi_CC3100MOD_SPI_TX_Channel.ChannelId = WIFI_SPI_TX_DMA_CHANNEL;
    BSP_WiFi_CC3100MOD_SPI_TX_Channel.Config.highPri = false;
    BSP_WiFi_CC3100MOD_SPI_TX_Channel.Config.enableInt = true;
    BSP_WiFi_CC3100MOD_SPI_TX_Channel.Config.select = DMAREQ_USART0_TXBL;
    BSP_WiFi_CC3100MOD_SPI_TX_Channel.CallBack.cbFunc = (DMA_FuncPtr_TypeDef) WiFi_CC3100MOD_SPI_TxDMAComplete;
    BSP_WiFi_CC3100MOD_SPI_TX_Channel.CallBack.userPtr = &BSP_WiFi_CC3100MOD_SPI_Handle;

    BSP_WiFi_CC3100MOD_SPI_RX_Channel.ChannelId = WIFI_SPI_RX_DMA_CHANNEL;
    BSP_WiFi_CC3100MOD_SPI_RX_Channel.Config.highPri = false;
    BSP_WiFi_CC3100MOD_SPI_RX_Channel.Config.enableInt = true;
    BSP_WiFi_CC3100MOD_SPI_RX_Channel.Config.select = DMAREQ_USART0_RXDATAV;
    BSP_WiFi_CC3100MOD_SPI_RX_Channel.CallBack.cbFunc = (DMA_FuncPtr_TypeDef) WiFi_CC3100MOD_SPI_RxDMAComplete;
    BSP_WiFi_CC3100MOD_SPI_RX_Channel.CallBack.userPtr = &BSP_WiFi_CC3100MOD_SPI_Handle;

    BSP_WiFi_CC3100MOD_SPI_Handle.Link1 = (void*)&BSP_WiFi_CC3100MOD_SPI_TX_Channel;
    BSP_WiFi_CC3100MOD_SPI_Handle.Link2 = (void*)&BSP_WiFi_CC3100MOD_SPI_RX_Channel;

    BSP_WiFi_CC3100MOD_SPI_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_DMA;

#else
    BSP_WiFi_CC3100MOD_SPI_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
#endif
    BSP_WiFi_CC3100MOD_SPI_Handle.TxDefaultData = 0xff;
    BSP_WiFi_CC3100MOD_SPI_Handle.Instance = WIFI_SERIAL_PORT;
    return RETCODE_OK;
}

/*
 * Refer to prototype for description
 */
Retcode_T BSP_WiFi_CC3100MOD_Enable(BSP_WiFi_CC3100MOD_InterruptCallback_T callback)
{
    Retcode_T retcode = RETCODE_OK;

    Board_WakeupPowerSupply2V5(WIFI_CC3100MOD);

    NVIC_DisableIRQ(GPIO_EVEN_IRQn);

    WiFi_CC3100MOD_ISRCallback = callback;

    Board_GPIOInterruptRegisterCallback(WIFI_INT_PIN, WiFi_INT_InterruptCallback);

    GPIO_IntConfig(WIFI_INT_PORT, WIFI_INT_PIN, WIFI_INT_EDGE_RISING, WIFI_INT_EDGE_FALLING, BSP_ENABLE);
    GPIO_IntClear(WIFI_INT_PIN);
    GPIO_IntEnable(WIFI_INT_PIN);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);

    GPIO_PinOutClear(VDD_WIFI_EN_PORT, VDD_WIFI_EN_PIN);
    GPIO_PinOutSet(WIFI_NHIB_PORT, WIFI_NHIB_PIN);
    Board_Delay(WIFI_SUPPLY_SETTING_DELAY_MS);

    GPIO_PinOutClear(WIFI_NRESET_PORT, WIFI_NRESET_PIN);
    Board_Delay(WIFI_MIN_RESET_DELAY_MS);
    GPIO_PinOutSet(WIFI_NRESET_PORT, WIFI_NRESET_PIN);
    Board_Delay(WIFI_PWRON_HW_WAKEUP_DELAY_MS);
    OStoBSPdelayFunc(WIFI_INIT_DELAY_MS);

    /*enable the clock for the USART interface*/
    CMU_ClockEnable(WIFI_SPI0_CLOCK, true);
    WIFI_SERIAL_PORT->CMD = USART_CMD_CLEARTX | USART_CMD_CLEARRX;
    /* Set interrupt priorities */
    NVIC_SetPriority(WIFI_RX_IRQN, WIFI_RX_INTERRUPT_PRIORITY);
    NVIC_SetPriority(WIFI_TX_IRQN, WIFI_TX_INTERRUPT_PRIORITY);
    /* clear pending interrupts */
    NVIC_ClearPendingIRQ(WIFI_RX_IRQN);
    NVIC_ClearPendingIRQ(WIFI_TX_IRQN);

    NVIC_EnableIRQ(WIFI_RX_IRQN);
    NVIC_EnableIRQ(WIFI_TX_IRQN);

    return retcode;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_WiFi_CC3100MOD_Disable(void)
{
    Retcode_T retcode = RETCODE_OK;

    /* Disable the USART clock*/
    CMU_ClockEnable(WIFI_SPI0_CLOCK, false);

    NVIC_DisableIRQ(BT_UART_RX_IRQn);
    NVIC_DisableIRQ(BT_UART_TX_IRQn);
    GPIO_IntClear(WIFI_INT_PIN);
    GPIO_IntDisable(WIFI_INT_PIN);
    Board_GPIOInterruptUnregisterCallback(WIFI_INT_PIN);
    WiFi_CC3100MOD_ISRCallback = NULL;
    /*Clear Reset Pin*/
    GPIO_PinOutClear(WIFI_NRESET_PORT, WIFI_NRESET_PIN);
    /* Clear Hibernate Pin*/
    GPIO_PinOutClear(WIFI_NHIB_PORT, WIFI_NHIB_PIN);
    /* Disable the WIFI module */
    GPIO_PinOutSet(VDD_WIFI_EN_PORT, VDD_WIFI_EN_PIN);
    /* Snooze the power supply if possible*/
    Board_SnoozePowerSupply2V5(WIFI_CC3100MOD);
    Board_Delay(WIFI_POWER_OFF_DELAY_MS);

    return retcode;

}
/*
 * Refer to interface header for description
 */
Retcode_T BSP_WiFi_CC3100MOD_Disconnect(void)
{
    GPIO_PinModeSet(VDD_WIFI_EN_PORT, VDD_WIFI_EN_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(WIFI_CSN_PORT, WIFI_CSN_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(WIFI_INT_PORT, WIFI_INT_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(WIFI_NHIB_PORT, WIFI_NHIB_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(WIFI_NRESET_PORT, WIFI_NRESET_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(WIFI_SPI0_MISO_PORT, WIFI_SPI0_MISO_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(WIFI_SPI0_MOSI_PORT, WIFI_SPI0_MOSI_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(WIFI_SPI0_SCK_PORT, WIFI_SPI0_SCK_PIN, gpioModeDisabled, 0);

    CMU_ClockEnable(WIFI_SPI0_CLOCK, true);
    USART0->ROUTE = _USART_ROUTE_RESETVALUE;
    CMU_ClockEnable(WIFI_SPI0_CLOCK, false);

    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_WiFi_CC3100MOD_Wakeup(void)
{

    GPIO_PinOutSet(WIFI_NHIB_PORT, WIFI_NHIB_PIN);
    Board_Delay(WIFI_HIB_WAKEUP_DELAY_MS);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_WiFi_CC3100MOD_Hibernate(void)
{

    GPIO_PinOutClear(WIFI_NHIB_PORT, WIFI_NHIB_PIN);
    Board_Delay(WIFI_MIN_HIB_DELAY_MS);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_WiFi_CC3100MOD_Reset(void)
{
    GPIO_PinOutClear(WIFI_NRESET_PORT, WIFI_NRESET_PIN);
    Board_Delay(WIFI_MIN_RESET_DELAY_MS);
    GPIO_PinOutSet(WIFI_NRESET_PORT, WIFI_NRESET_PIN);

    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
HWHandle_T BSP_WiFi_CC3100MOD_GetSPIHandle(void)
{
    /*
     * Initialize the Handle and return it back to the caller
     */
    return (HWHandle_T) &BSP_WiFi_CC3100MOD_SPI_Handle;
}

/*
 * Refer to interface header for description
 */
HWHandle_T BSP_WiFi_CC3100MOD_GetUartHandle(void)
{
    return 0;
}

Retcode_T BSP_WiFi_CC3100MOD_SetCSLow()
{
    GPIO_PinOutClear(WIFI_CSN_PORT, WIFI_CSN_PIN);
    return RETCODE_OK;
}
Retcode_T BSP_WiFi_CC3100MOD_SetCSHigh()
{
    GPIO_PinOutSet(WIFI_CSN_PORT, WIFI_CSN_PIN);
    return RETCODE_OK;
}
/*
 * Refer to interface header for description
 */
void WiFi_CC3100MOD_SPIRxISR(void)
{
    /* All UART interrupt processing is to be done at the MCU level */
    MCU_SPI_IRQ_RX_Callback((SPI_T) &BSP_WiFi_CC3100MOD_SPI_Handle);
}

/*
 * Refer to interface header for description
 */
void WiFi_CC3100MOD_SPITxISR(void)
{
    /* All UART interrupt processing is to be done at the MCU level */
    MCU_SPI_IRQ_TX_Callback((SPI_T) &BSP_WiFi_CC3100MOD_SPI_Handle);
}

#if BCDS_FEATURE_DMA
/*
 * Refer to interface header for description
 */
void WiFi_CC3100MOD_SPI_RxDMAComplete(uint32_t channel, bool primary, void * user)
{
    BCDS_UNUSED(channel);
    BCDS_UNUSED(primary);
    BCDS_UNUSED(user);

    /* All UART interrupt processing is to be done at the MCU level */
    MCU_SPI_DMA_RX_Callback((SPI_T) &BSP_WiFi_CC3100MOD_SPI_Handle);
}

/*
 * Refer to interface header for description
 */
void WiFi_CC3100MOD_SPI_TxDMAComplete(uint32_t channel, bool primary, void * user)
{
    BCDS_UNUSED(channel);
    BCDS_UNUSED(primary);
    BCDS_UNUSED(user);

    /* All UART interrupt processing is to be done at the MCU level */
    MCU_SPI_DMA_TX_Callback((SPI_T) &BSP_WiFi_CC3100MOD_SPI_Handle);
}

#endif



static void WiFi_INT_InterruptCallback(void)
{
    WiFi_CC3100MOD_ISRCallback();

}
#endif /* BCDS_FEATURE_BSP_WiFi_CC3100MOD */

