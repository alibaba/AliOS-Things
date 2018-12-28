/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Header for driver hw msp module

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
/******************************************************************************
 * @file    hw_msp.h
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   Header for driver hw msp module
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

#ifndef __HW_MSP_H__
#define __HW_MSP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hw_conf.h"

#if 0
/* Exported types ------------------------------------------------------------*/
typedef enum
{
  e_LOW_POWER_RTC = (1 << 0),
  e_LOW_POWER_GPS = (1 << 1),
  e_LOW_POWER_UART = (1 << 2), /* can be used to forbid stop mode in case of uart Xfer*/
} e_LOW_POWER_State_Id_t;
#endif

/* Exported constants --------------------------------------------------------*/

#define VDDA_VREFINT_CAL         ((uint32_t) 3000)
#define BAT_CR2032               ((uint32_t) 3000)
#define VDD_BAT                  BAT_CR2032
#define VDD_MIN                  1800

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/**
 * @brief GPIOs Macro
 */

#define RCC_GPIO_CLK_ENABLE(__GPIO_PORT__)                                          \
  do {                                                                              \
    switch (__GPIO_PORT__)                                                          \
    {                                                                               \
      case GPIOA_BASE: LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA); break;    \
      case GPIOB_BASE: LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB); break;    \
      case GPIOC_BASE: LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC); break;    \
      case GPIOD_BASE: LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD); break;    \
      case GPIOH_BASE: default:  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH); \
    }                                                    \
  } while (0)

#define RCC_GPIO_CLK_DISABLE(__GPIO_PORT__)               \
  do {                                                    \
    switch (__GPIO_PORT__)                                \
    {                                                     \
      case GPIOA_BASE: LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOA); break;    \
      case GPIOB_BASE: LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOB); break;    \
      case GPIOC_BASE: LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOC); break;    \
      case GPIOD_BASE: LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOD); break;    \
      case GPIOH_BASE: default:  LL_IOP_GRP1_DisableClock(LL_IOP_GRP1_PERIPH_GPIOH); \
    }                                                    \
  } while (0)

/* Exported functions ------------------------------------------------------- */

/**
 * @brief  This function return the battery level
 * @param  None
 * @retval The battery level  0 (very low) to 254 (fully charged)
 */
uint8_t HW_GetBatteryLevel(void);

/**
 * @brief  Initializes the boards peripherals.
 * @param  None
 * @retval None
 */
void HW_Init(void);

/**
 * @brief  De-initializes the target board peripherals to decrease power consumption.
 * @param  None
 * @retval None
 */
void HW_DeInit(void);

/**
 * @brief  This function return a random seed
 * @note   Based on the device unique ID
 * @param  None
 * @retval Unique number, used as a random seed
 */
uint32_t HW_GetRandomSeed(void);

/**
 * @brief  Gets the board 64 bits unique ID
 * @param  Pointer to an array that will contain the Unique ID
 * @retval None
 */
void HW_GetUniqueId(uint8_t *id);

/**
 * @brief  Enters Low Power Stop Mode
 * @note   ARM exists the function when waking up
 * @param  None
 * @retval None
 */
void HW_EnterStopMode(void);

/**
 * @brief  Exit Low Power Stop Mode
 * @note   Enable the pll at 32MHz
 * @param  None
 * @retval None
 */
void HW_ExitStopMode(void);

/**
 * @brief  Enters Low Power Sleep Mode
 * @note   ARM exists the function when waking up
 * @param  None
 * @retval None
 */
void HW_EnterSleepMode(void);

/**
 * @brief  Configures the sytem Clock at start-up, as follow :
 *            System Clock source            = PLL (HSI)
 *            SYSCLK(Hz)                     = 32000000
 *            HCLK(Hz)                       = 32000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            HSI Frequency(Hz)              = 16000000
 *            PLLMUL                         = 6
 *            PLLDIV                         = 3
 *            Flash Latency(WS)              = 1
 * @param  None
 * @retval None
 */
void SystemClock_Config(void);

/**
 * @brief  Configure all GPIO's to Analog input to reduce the power consumption
 * @param  None
 * @retval None
 */
void HW_GpioInit(void);

void SYS_LED_OFF(void);
void SYS_LED_ON(void);
uint32_t HW_Get_MFT_ID(void);
uint32_t HW_Get_MFT_Model(void);
uint32_t HW_Get_MFT_Rev(void);
uint32_t HW_Get_MFT_SN(void);
bool HW_Set_MFT_Baud(uint32_t baud);
uint32_t HW_Get_MFT_Baud(void);

#ifdef __cplusplus
}
#endif

#endif /* __HW_MSP_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
