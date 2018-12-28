/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: contains hardaware configuration Macros and Constants

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
/******************************************************************************
 * @file    eml3047_hw_conf.h
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   contains hardaware configuration Macros and Constants
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EML3047_HW_CONF_H__
#define __EML3047_HW_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* LORA I/O definition */

#define RADIO_RESET                               PB_4

#define RADIO_MOSI                                PA_7

#define RADIO_MISO                                PA_6

#define RADIO_SCLK                                PA_5

#define RADIO_NSS                                 PA_4

#define RADIO_DIO_0                               PA_12

#define RADIO_DIO_1                               PA_11

#define RADIO_DIO_2                               PA_10

#define RADIO_DIO_3                               PA_9

#define RADIO_DIO_4                               PA_8

#define RADIO_DIO_5                               PB_1

//#define RADIO_TCXO_VCC_PORT                       GPIOA
//#define RADIO_TCXO_VCC_PIN                        GPIO_PIN_12

#define RADIO_RF_FEM_CPS                          PB_5

#define RADIO_ANT_SWITCH                          PB_6

/*  SPI MACRO redefinition */

#define SPI_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()

#define SPI_AF                          GPIO_AF0_SPI1

/* ADC MACRO redefinition */
#define BAT_LEVEL_PORT                   GPIOA
#define BAT_LEVEL_PIN                    GPIO_PIN_0
#define ADC_READ_CHANNEL                 ADC_CHANNEL_0
#define ADCCLK_ENABLE()                 __HAL_RCC_ADC1_CLK_ENABLE();
#define ADCCLK_DISABLE()                __HAL_RCC_ADC1_CLK_DISABLE();

#define APP_WAKE_PORT                  GPIOB
#define APP_WAKE_PIN                   GPIO_PIN_7
#define APP_TO_SLEEP                   1

#define APP_LED_PORT                   GPIOB
#define APP_LED_PIN                    GPIO_PIN_0

/* --------------------------- RTC HW definition -------------------------------- */

#define RTC_OUTPUT       DBG_RTC_OUTPUT

#define RTC_Alarm_IRQn              RTC_IRQn
/* --------------------------- UART HW definition -------------------------------*/

/* Definition for UARTx clock resources */
#define UARTX                           LPUART1
#define UARTX_CLK_ENABLE()              LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1)
#define UARTX_RX_GPIO_CLK_ENABLE()      LL_IOP_GRP1_EnableClock(RCC_IOPENR_GPIOBEN)
#define UARTX_TX_GPIO_CLK_ENABLE()      LL_IOP_GRP1_EnableClock(RCC_IOPENR_GPIOBEN)

#define UARTX_FORCE_RESET()             LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_LPUART1)
#define UARTX_RELEASE_RESET()           LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_LPUART1)

#define UARTX_TX_PIN                    GPIO_PIN_2
#define UARTX_TX_GPIO_PORT              GPIOA
#define UARTX_TX_AF                     GPIO_AF4_USART2
#define UARTX_RX_PIN                    GPIO_PIN_3
#define UARTX_RX_GPIO_PORT              GPIOA
#define UARTX_RX_AF                     GPIO_AF4_USART2

/* Definition for USARTx's NVIC */
#define UARTX_IRQn                      LPUART1_IRQn
#define UARTX_IRQHandler                LPUART1_IRQHandler

#ifdef __cplusplus
}
#endif

#endif /* __32L082MLM_HW_CONF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
