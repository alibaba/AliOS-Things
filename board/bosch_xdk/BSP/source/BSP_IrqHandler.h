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
 * @brief interrupt handlers used within the board support package and overwriting
 * weak definition of ARM startup file.
 *
 *Type of Interrupt        | Predefined IRQ name
 *-------------------------|------------------
 *Core interrupt|Reset_Handler
 *Core interrupt|NMI_Handler
 *Core interrupt|HardFault_Handler
 *Core interrupt|MemManage_Handler
 *Core interrupt|BusFault_Handler
 *Core interrupt|UsageFault_Handler
 *Core interrupt|Default_Handler
 *Core interrupt|Default_Handler
 *Core interrupt|Default_Handler
 *Core interrupt|Default_Handler
 *Core interrupt|SVC_Handler
 *Core interrupt|DebugMon_Handler
 *Core interrupt|Default_Handler
 *Core interrupt|PendSV_Handler
 *Core interrupt|SysTick_Handler
 *External interrupt | DMA_IRQHandler
 *External interrupt|GPIO_EVEN_IRQHandler
 *External interrupt|TIMER0_IRQHandler
 *External interrupt|USART0_RX_IRQHandler
 *External interrupt|USART0_TX_IRQHandler
 *External interrupt|USB_IRQHandler
 *External interrupt|ACMP0_IRQHandler
 *External interrupt|ADC0_IRQHandler
 *External interrupt|DAC0_IRQHandler
 *External interrupt|I2C0_IRQHandler
 *External interrupt|GPIO_ODD_IRQHandler
 *External interrupt|TIMER1_IRQHandler
 *External interrupt|TIMER2_IRQHandler
 *External interrupts|TIMER3_IRQHandler
 *External interrupt|USART1_RX_IRQHandler
 *External interrupt|USART1_TX_IRQHandler
 *External interrupt|LESENSE_IRQHandler
 *External interrupt|USART2_RX_IRQHandler
 *External interrupt|USART2_TX_IRQHandler
 *External interrupt|UART0_RX_IRQHandler
 *External interrupt|UART0_TX_IRQHandler
 *External interrupt|UART1_RX_IRQHandler
 *External interrupt|UART1_TX_IRQHandler
 *External interrupt|LEUART0_IRQHandler
 *External interrupt|LEUART1_IRQHandler
 *External interrupt|LETIMER0_IRQHandler
 *External interrupt|PCNT0_IRQHandler
 *External interrupt|PCNT1_IRQHandler
 *External interrupt|PCNT2_IRQHandler
 *External interrupt|RTC_IRQHandler
 *External interrupt|BURTC_IRQHandler
 *External interrupts|CMU_IRQHandler
 *External interrupt|VCMP_IRQHandler
 *External interrupt|LCD_IRQHandler
 *External interrupt|MSC_IRQHandler
 *External interrupt|AES_IRQHandler
 *External interrupt|EBI_IRQHandler
 *External interrupt|EMU_IRQHandler
 *
 *@note in order for this file to be linked correctly the option <b>--whole-archive</b>
 *should be used for the linker script
 *
 */

#ifndef IrqHandler_H
#define IrqHandler_H

/**
 * @brief  SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void);

/**
 * @brief GPIO EVEN interrupt handler.
 * @details Interrupt handler clears all IF even
 * flags and call the dispatcher passing the flags which triggered the interrupt.
 * @note GPIO_EVEN_IRQHandler is used for all the EVEN GPIO pins capable and configured to generate interrupts in XDK
 */
void GPIO_EVEN_IRQHandler(void);

/**
 * @brief  GPIO ODD interrupt handler.
 * @details Interrupt handler clears all IF odd flags
 * and call the dispatcher passing the flags which triggered the interrupt.
 * @note GPIO_ODD_IRQHandler is used for all the ODD GPIO pins capable and configured to generate interrupts in XDK
 */
void GPIO_ODD_IRQHandler(void);

#if BCDS_FEATURE_BSP_BT_EM9301
/**
 * @brief  UART0 Tx interrupt handler.
 * @details Interrupt handler forwards all UART transmit related interrupts to
 * the MCU driver for processing.
 * @note UART0_TX_IRQHandler is used for the BT LE module in XDK
 */
void UART0_TX_IRQHandler(void);

/**
 * @brief  UART0 Rx interrupt handler.
 * @details Interrupt handler forwards all UART receive related interrupts to
 * the MCU driver for processing.
 * @note UART0_RX_IRQHandler is used for the BT LE module in XDK
 */
void UART0_RX_IRQHandler(void);

#endif

#if BCDS_FEATURE_BSP_WIFI_CC3100MOD
/**
 * @brief  USART0 Tx interrupt handler.
 * @details Interrupt handler forwards all USART transmit related interrupts to
 * the MCU driver for processing.
 * @note USART0_TX_IRQHandler is used for the wlan module in XDK
 */
void USART0_TX_IRQHandler(void);

/**
 * @brief  USART0 Rx interrupt handler.
 * @details Interrupt handler forwards all USART receive related interrupts to
 * the MCU driver for processing.
 * @note USART0_RX_IRQHandler is used for the wlan module in XDK
 */
void USART0_RX_IRQHandler(void);

#endif /* BCDS_FEATURE_BSP_WIFI_CC3100MOD */

#if BCDS_FEATURE_BSP_SD_CARD
/**
 * @brief  USART1 Tx interrupt handler.
 * @details Interrupt handler forwards all USART transmit related interrupts to
 * the MCU driver for processing.
 * @note USART1_TX_IRQHandler is used for the SD card module in XDK
 */
void USART1_TX_IRQHandler(void);

/**
 * @brief  USART1 Rx interrupt handler.
 * @details Interrupt handler forwards all USART receive related interrupts to
 * the MCU driver for processing.
 * @note USART1_RX_IRQHandler is used for the SD card module in XDK
 */
void USART1_RX_IRQHandler(void);

#endif /* BCDS_FEATURE_BSP_SD_CARD */

#if BCDS_FEATURE_BSP_SENSOR_NODE
/**
 * @brief  I2C0 interrupt handler.
 * @details Interrupt handler forwards all I2C receive related interrupts to
 * the MCU driver for processing.
 * @note I2C0_IRQHandler is used for the all the sensors node modules in XDK
 */
void I2C0_IRQHandler(void);
#endif /*BCDS_FEATURE_BSP_SENSOR_NODE*/

#if BCDS_FEATURE_BSP_EXTENSIONPORT
/**
* @brief  I2C1 interrupt handler.
 * @details Interrupt handler forwards all I2C receive related interrupts to
 * the MCU driver for processing.
 * @note I2C1_IRQHandler is used for the XDK extension port
 */
void I2C1_IRQHandler(void);

/**
 * @brief  USART2 Tx interrupt handler.
 * @details Interrupt handler forwards all USART transmit related interrupts to
 * the MCU driver for processing.
 * @note USART2_TX_IRQHandler is used for the Extension Bus SPI module in XDK
 */
void USART2_TX_IRQHandler(void);

/**
 * @brief  USART2 Rx interrupt handler.
 * @details Interrupt handler forwards all USART receive related interrupts to
 * the MCU driver for processing.
 * @note USART2_RX_IRQHandler is used for the Extension Bus SPI module in XDK
 */
void USART2_RX_IRQHandler(void);

/**
 * @brief  UART1 Tx interrupt handler.
 * @details Interrupt handler forwards all UART transmit related interrupts to
 * the MCU driver for processing.
 */
void UART1_TX_IRQHandler(void);

/**
 * @brief  UART1 Rx interrupt handler.
 * @details Interrupt handler forwards all UART transmit related interrupts to
 * the MCU driver for processing.
 */
void UART1_RX_IRQHandler(void);

#endif

/*@todo workaround to address weak linkage of SysTick_Handler defined in IrqHandler.c file */
Retcode_T IrqHandler_SysTickDummyHandler(void);

#endif /* IrqHandler_H */

