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
 * @brief  Implementation of HW Interrupt handler
 * @file
 */
#include "BCDS_HAL.h"
#include "BSP_BoardType.h"
#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BSP_BT_EM9301.h"
#include "BSP_IrqHandler.h"
#include "BSP_SDCard.h"
#include "BSP_SensorNode.h"
#include "BSP_WiFi_cc3100mod.h"
#include "BSP_LoRa_RN2xx3.h"
#include "BSP_UMTS_LisaU2.h"
#include "BSP_UMTS_QuectelUC20.h"
#include "BspExtensionPort.h"
#include "BCDS_MCU_Timer.h"
#include "BCDS_MCU_Timer_Handle.h"
#include "BspAdc.h"

#if BCDS_FEATURE_ADC
/* Please note this handler is not guarded by any feature macro because it is used for both internal and Extension board*/
void ADC0_IRQHandler(void)
{
    BSP_Adc_HandlerIsr();
}
#endif  /* BCDS_FEATURE_ADC */
#if 0
#if BCDS_FEATURE_HAL_TICK_HANDLER
/*
 * Refer to prototype for description
 */
void SysTick_Handler(void)
{
    if (NULL != SysTickOsPreProcessing)
    {
        SysTickOsPreProcessing();
    }

    Board_IncTick();

    if (NULL != SysTickOsPostProcessing)
    {
        SysTickOsPostProcessing();
    }
}
#endif
#endif
/*
 * Refer to prototype for description
 */
void GPIO_EVEN_IRQHandler(void)
{
    uint32_t iflags;

    /* Get all even interrupts. */
    iflags = GPIO_IntGetEnabled() & BSP_GPIO_INT_EVEN_MASK;

    /* Clean only even interrupts. */
    GPIO_IntClear(iflags);

    Board_GPIOInterruptDispatch(iflags);
}

/*
 * Refer to prototype for description
 */
void GPIO_ODD_IRQHandler(void)
{
    uint32_t iflags;

    /* Get all odd interrupts. */
    iflags = GPIO_IntGetEnabled() & BSP_GPIO_INT_ODD_MASK;

    /* Clean only even interrupts. */
    GPIO_IntClear(iflags);

    Board_GPIOInterruptDispatch(iflags);
}

#if BCDS_FEATURE_BSP_BT_EM9301
/*
 * Refer to interface header for description
 */
void UART0_TX_IRQHandler(void)
{
    BT_EM9301_UART_TxISR();
}
/*
 * Refer to interface header for description
 */
void UART0_RX_IRQHandler(void)
{
    BT_EM9301_UART_RxISR();
}
#endif

#if BCDS_FEATURE_BSP_WIFI_CC3100MOD
/*
 * Refer to interface header for description
 */
void USART0_TX_IRQHandler(void)
{
    WiFi_CC3100MOD_SPITxISR();
}
/*
 * Refer to interface header for description
 */
void USART0_RX_IRQHandler(void)
{

    WiFi_CC3100MOD_SPIRxISR();
}
#endif

#if BCDS_FEATURE_BSP_SENSOR_NODE
/*
 * Refer to interface header for description
 */
void I2C0_IRQHandler(void)
{
    SensorNode_I2CISR();
}

#endif

#if BCDS_FEATURE_BSP_EXTENSIONPORT
/*
 * Refer header BSP_IrqHandler.h for description
 */
void I2C1_IRQHandler(void)
{
    BSP_ExtensionPort_I2cIsr();
}

/*
 * Refer header BSP_IrqHandler.h for description
 */
void USART2_TX_IRQHandler(void)
{
    BSP_ExtensionPort_SPITxISR();
}

/*
 * Refer header BSP_IrqHandler.h for description
 */
void USART2_RX_IRQHandler(void)
{
    BSP_ExtensionPort_SPIRxISR();
}

/*
 * Refer header BSP_IrqHandler.h for description
 */
void UART1_TX_IRQHandler(void)
{
    BSP_ExtensionPort_UARTTxIsr();
}

/*
 * Refer header BSP_IrqHandler.h for description
 */
void UART1_RX_IRQHandler(void)
{
    BSP_ExtensionPort_UARTRxIsr();
}

#endif

#if BCDS_FEATURE_BSP_SD_CARD
/*
 * Refer to interface header for description
 */
void USART1_TX_IRQHandler(void)
{
    SDCard_SPITxISR();
}
/*
 * Refer to interface header for description
 */
void USART1_RX_IRQHandler(void)
{
    SDCard_SPIRxISR();
}
#endif

#if BCDS_FEATURE_EFM32_BURTC
void BURTC_IRQHandler(void)
{
    MCU_Timer_HandleBurtcIrq();
}
#endif

#if BCDS_FEATURE_EFM32_RTC
void RTC_IRQHandler(void)
{
    MCU_Timer_HandleRtcIrq();
}
#endif

/** @todo workaround to address weak linkage of SysTick_Handler defined in IrqHandler.c file */

Retcode_T IrqHandler_SysTickDummyHandler(void)
{
    return RETCODE_OK;
}
